/**
 * @file    f_measurement.c
 * @brief   Initialising interrupt, handling the ISR events via a seperate task, calculating the frequency
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "f_measurement.h"

#include "timer_drv.h"

#define TAG "f_measurement"

static xQueueHandle isr_queue = NULL;  // Queu for sending measured time

/**
 * @brief Interrupt Service Routine Handler
 */
static void IRAM_ATTR isr_handler(void *arg) {
    static uint64_t isr_count_total = 0;  // Number of timer ticks since timer init
    static uint64_t isr_pulses = 0;       // Number of interrupt pulses (isr calls)
    static uint64_t isr_count = 0;        // Number of timer ticks since last reading

    if (isr_pulses == 1) {
        isr_count_total = drv_timer_get_count_isr();  // Reset the isr_count_total with the first pulse
    }

    if ((isr_pulses % PULSES_PER_MEAS) == 0) {                    // Every PULSES_PER_MEAS
        isr_count = drv_timer_get_count_isr() - isr_count_total;  // Calc timer ticks since last reading
        isr_count_total = drv_timer_get_count_isr();
        xQueueSendFromISR(isr_queue, &isr_count, NULL);  // Send it to the queue
    }

    isr_pulses++;  // Increment pulses count each time ISR is executed
}

/**
 * @brief Read measured frequency if a new value is available
 * @return Frequency or -1 if no new value is available
 */
float f_measurement_get_val() {
    uint64_t count;
    float frequency = -1.0;
    if (xQueueReceive(isr_queue, &count, portMAX_DELAY) == pdTRUE) {
        frequency = (float)(((40 * 1000000 * (uint64_t)PULSES_PER_MEAS)) / (float)count);  // Timer f: 40 MHz, PULSES_PER_MEAS pulses
        frequency = (frequency > 51.0) ? 51.0 : frequency;                                 // Set upper limit to 51 Hz
        frequency = (frequency < 49.0) ? 49.0 : frequency;                                 // Set lowe limit to 49 Hz

        // float time = (count * (float)2.5 / (float)1000000.0);
        // ESP_LOGD(TAG, "| Count: %llu | Time: %d ms | Freq: %.3lf Hz ", count, 0, frequency);
    }
    return frequency;
}

/**
 * @brief Interrupt GPIO pin configuration
 * @param gpio_input_pin_sel bit mask of the pin to be used for an interrupt
 * @return Error code
 */
static esp_err_t intr_gpio_config(uint64_t gpio_input_pin_sel) {
    gpio_config_t io_conf = {
        // GPIO Configuration structure
        .pin_bit_mask = gpio_input_pin_sel,  // Bit mask of the pin to be used for an interrupt
        .mode = GPIO_MODE_INPUT,             // Set as input mode
        .pull_up_en = 1,                     // Enable pull-up mode
        .intr_type = GPIO_INTR_POSEDGE       // Interrupt of rising edge
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));  // Configure GPIO with the given settings
    ESP_LOGI(TAG, "INTR GPIO Configured");
    return ESP_OK;
}

/**
 * @brief Initialise frequency measurement: Interrupt (GPIO, Task, Que, ISR) and Timer
 * @param gpio_interrupt pin to be used for the interrupt
 * @return Error code
 */
esp_err_t f_measurement_init(uint64_t gpio_interrupt) {
    esp_err_t err = ESP_OK;
    ESP_ERROR_CHECK(drv_timer_init());

    uint64_t gpio_input_pin_select = (1ULL << gpio_interrupt);
    err = intr_gpio_config(gpio_input_pin_select);  // Initialise gpio for the interrupt
    ESP_ERROR_CHECK(err);

    // Create a queue to handle timer measurements from the isr
    isr_queue = xQueueCreate(10, sizeof(uint64_t));

    err = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);  // Install gpio isr service
    ESP_ERROR_CHECK(err);

    err = gpio_isr_handler_add(gpio_interrupt, isr_handler, NULL);  // Hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "ISR Service installed, handler added, interrupt task created");
    return ESP_OK;
}

/**
 * @brief Task responsible for simulating the zero-crossing output
 * @param *param pointer to the uint64_t var with the pin to be used for the test
 */
static void ZCO_sim_task(void *param) {
    vTaskDelay(200 / portTICK_PERIOD_MS);  // Initial delay to ensure smooth execution of the task
    struct timeval time;
    gettimeofday(&time, NULL);  // Copy current sys time to sys_time struct
    srand(time.tv_sec);         // Initialise seed for the rand function

    uint64_t gpio_zco = (uint64_t)param;
    static uint8_t test_pin_state = 0;  // Variable with pin state (default 0)

    for (int i = 0; i < 20; i++) {
        for (int i = 0; i < ((PULSES_PER_MEAS * 2) - 2); i++) {
            gpio_set_level(gpio_zco, test_pin_state);  // Set the GPIO level according to the state
            test_pin_state = !test_pin_state;          // Toggle the pin state
            vTaskDelay(10 / portTICK_PERIOD_MS);       // Pulse period (high and low) 20 ms, frequency 50 Hz
        }

        const TickType_t rand_f = ((10 * (rand() % 3)) / portTICK_PERIOD_MS);  // Randomize frequency by inserting 0/1/2 ticks of delay
        for (int i = 0; i < 2; i++) {
            gpio_set_level(gpio_zco, test_pin_state);  // Set the GPIO level according to the state
            test_pin_state = !test_pin_state;          // Toggle the LED state
            vTaskDelay(rand_f);                        // Non blocking delay
        }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait for all measurement to finish
    ESP_LOGW(TAG, "-------- End frequency measurement test --------\n");
    while (true) {
    }  // Infinite loop to keep task running
}

/**
 * @brief Test frequency measurement capabilities by simulating the zero-crossing output (ZCO)
 * @param gpio_zco pin to be used for the test
 * @return Error code
 */
esp_err_t f_measurement_test(const uint64_t gpio_zco) {
    gpio_reset_pin(gpio_zco);                                                          // Set the GPIO as a push/pull output
    gpio_set_direction(gpio_zco, GPIO_MODE_OUTPUT);                                    // Set the GPIO as an output
    xTaskCreate(ZCO_sim_task, "ZCO_sim_task", 2048, (void *const)gpio_zco, 10, NULL);  // Start interrupt task
    ESP_LOGI(TAG, "Frequency measurement test initialised");

    return ESP_OK;
}
