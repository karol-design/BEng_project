#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "interrupts_drv.h"

#define TAG "Interrupt_drv"

#define GPIO_INTERRUPT 4
#define GPIO_INPUT_PIN_SEL (1ULL << GPIO_INTERRUPT)
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

/**
 * @brief Interrupt Service Routine Handler
 */
static void IRAM_ATTR isr_handler(void *arg) {
    xQueueSendFromISR(gpio_evt_queue, (uint32_t)(1), NULL);
}

/**
 * @brief Task handling messages sent to the que by the ISR
 */
static void interrupt_task(void *arg) {
    uint32_t data;
    while(true) {
        if (xQueueReceive(gpio_evt_queue, &data, portMAX_DELAY)) {
            ESP_LOGI(TAG, "ISR Called");
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
    
    return ESP_OK;
}

/**
 * @brief Interrupt initialisation (GPIO, Task, Que, ISR)
 * @return Error code
 */
esp_err_t interrupt_init() {
    esp_err_t err = ESP_OK;

    intr_gpio_config();
    ESP_ERROR_CHECK(err);

    // Create a queue to handle gpio events from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));                 
    xTaskCreate(interrupt_task, "interrupt_task", 2048, NULL, 10, NULL); // Start interrupt task

    err = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT); // Install gpio isr service
    ESP_ERROR_CHECK(err);

    err = gpio_isr_handler_add(GPIO_INTERRUPT, isr_handler, NULL); // Hook isr handler for specific gpio pin
    ESP_ERROR_CHECK(err);

    return ESP_OK;
}