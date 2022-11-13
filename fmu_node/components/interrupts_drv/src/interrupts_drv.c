/**
 * @file    interrupts_drv.c
 * @brief   Initialising interrupt and ISR, handling the ISR events via a seperate task
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "interrupts_drv.h"
#include "timers_drv.h"

#define TAG "Interrupt_drv"

#define GPIO_INTERRUPT 4    // Pin to be used for the interrupt
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_INTERRUPT)
#define ESP_INTR_FLAG_DEFAULT 0

#define PULSES_PER_MEAS 10  // Number of interrupt pulses for one time measurement

static xQueueHandle isr_queue = NULL;   // Queu for sending measured time

/**
 * @brief Interrupt Service Routine Handler
 */
static void IRAM_ATTR isr_handler(void *arg) {
    static uint64_t isr_count_total = 0;    // Number of timer ticks since timer init
    static uint64_t isr_pulses = 0;         // Number of interrupt pulses (isr calls)
    static uint64_t isr_count = 0;          // Number of timer ticks since last reading

    if((isr_pulses % PULSES_PER_MEAS) == 0) {               // Every PULSES_PER_MEAS
        isr_count = drv_timer_get_count_isr() - isr_count_total;  // Calc timer ticks since last reading
        isr_count_total = drv_timer_get_count_isr();
        xQueueSendFromISR(isr_queue, &isr_count, NULL);     // Send it to the queue
    }
    
    isr_pulses++;   // Increment pulses count each time ISR is executed
}

/**
 * @brief Task handling timer-based measurements sent to the queue by the ISR
 */
static void interrupt_task(void *arg) {
    uint64_t count;
    while(true) {
        if (xQueueReceive(isr_queue, &count, portMAX_DELAY) == pdTRUE) { 
            float frequency = ((float)(40*1000000*10) / (float)count);    // Timer tick f: 40 MHz, 10 pulses
            ESP_LOGI(TAG, "Measured time: %llu, freq: %lf", count, frequency);  
        }
    }
}

/**
 * @brief Interrupt GPIO pin configuration
 * @return Error code
 */
static esp_err_t intr_gpio_config() {
    gpio_config_t io_conf = {               // GPIO Configuration structure
        .pin_bit_mask = GPIO_INPUT_PIN_SEL, // Bit mask of the pin to be used for an interrupt
        .mode = GPIO_MODE_INPUT,            // Set as input mode
        .pull_up_en = 1,                    // Enable pull-up mode
        .intr_type = GPIO_INTR_POSEDGE      // Interrupt of rising edge
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf)); // Configure GPIO with the given settings
    ESP_LOGI(TAG, "INTR GPIO Configured");
    return ESP_OK;
}

/**
 * @brief Interrupt initialisation (GPIO, Task, Que, ISR)
 * @return Error code
 */
static esp_err_t drv_interrupt_init() {
    esp_err_t err = ESP_OK;

    err = intr_gpio_config();   // Initialise gpio for the interrupt
    ESP_ERROR_CHECK(err);

    // Create a queue to handle timer measurements from isr
    isr_queue = xQueueCreate(10, sizeof(uint64_t));                 
    xTaskCreate(interrupt_task, "interrupt_task", 2048, NULL, 10, NULL); // Start interrupt task

    err = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT); // Install gpio isr service
    ESP_ERROR_CHECK(err);

    err = gpio_isr_handler_add(GPIO_INTERRUPT, isr_handler, NULL); // Hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(err);
    
    ESP_LOGI(TAG, "ISR Service installed, handler added, interrupt task created");
    return ESP_OK;
}

static void test_task(void *arg) {
    static uint8_t test_pin_state = 0;             // Variable with LED state (default 0)
    while (1) {
        gpio_set_level(12, test_pin_state);   // Set the GPIO level according to the state
        test_pin_state = !test_pin_state;     // Toggle the LED state
        vTaskDelay(10 / portTICK_PERIOD_MS); // Non blocking delay
    }
}

static esp_err_t drv_test_init() {
    gpio_reset_pin(12);                         // Set the GPIO as a push/pull output
    gpio_set_direction(12, GPIO_MODE_OUTPUT);   // Set the GPIO as an output
    xTaskCreate(test_task, "test_task", 2048, NULL, 10, NULL); // Start interrupt task

    return ESP_OK;
}

/**
 * @brief Initialise frequency measurement (Interrupt and timer)
 * @return Error code
 */
esp_err_t f_meaurement_init() {
    ESP_ERROR_CHECK(drv_timer_init());
    ESP_ERROR_CHECK(drv_interrupt_init());
    ESP_ERROR_CHECK(drv_test_init());

    return ESP_OK;
}