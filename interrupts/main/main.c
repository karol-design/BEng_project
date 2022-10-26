#include <stdio.h>
#include "esp_log.h"    // Logging macro
#include "esp_check.h"  // Error checking macro

#define TAG "MAIN"

#define GPIO_INPUT_IO_0 4
#define GPIO_INPUT_PIN_SEL (1ULL<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR isr_handler()
{
    ESP_LOGI(TAG, "Interrupt!");
}

void app_main(void)
{
    esp_err_t err = ESP_OK;


    ESP_LOGI(TAG, "This is a message from the main.c");
    ESP_LOGI(TAG, "Value returned by the driver function is %d", func());

    // GPIO Configuration
    gpio_config_t io_conf = {
        .pin_bit_mask = GPIO_INPUT_PIN_SEL;  // Bit mask of the pin to be used for an interrupt
        .mode = GPIO_MODE_INPUT;             // Set as input mode
        .pull_up_en = 1;                     // Enable pull-up mode
        .intr_type = GPIO_INTR_POSEDGE;      // Interrupt of rising edge
    }

    err = gpio_config(&io_conf);  // Configure GPIO with the given settings
    ESP_ERROR_CHECK(err);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);            // Install gpio isr service
    gpio_isr_handler_add(GPIO_INPUT_IO_0, isr_handler, void);   // Hook isr handler for specific gpio pin
}
