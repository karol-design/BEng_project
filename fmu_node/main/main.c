/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "esp_check.h"
#include "esp_log.h"
#include "f_measurement.h"
#include "nvs_flash.h"
#include "wifi_drv.h"

#define TAG "app"

#define ZCO_PIN 4
#define TEST_PIN 12

void app_main(void) {
    esp_err_t err = ESP_OK;

    err = nvs_flash_init();  // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS flash memory
        err = nvs_flash_init();              // And try initialising it again
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(wifi_drv_init());  // Initialise WiFi

    vTaskDelay(10000 / portTICK_RATE_MS);

    ESP_LOGI(TAG, "Frequency measurement test");
    ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));   // Initialise frequency measurement
    ESP_ERROR_CHECK(f_measurement_test(TEST_PIN));  // Initialise ZCO simulation & freq. measurement test

    while (true) {
        float frequency = f_measurement_get_val();  // Read frequency
        if (frequency != -1.0) {                    // Check if a new value was available
            ESP_LOGI(TAG, " Freq: %lf Hz ", frequency);
        }
    }
}