/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>

#include "esp_check.h"
#include "esp_log.h"
#include "f_measurement.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "systime.h"
#include "wifi_drv.h"

#define TAG "app"
#define ZCO_PIN 4
#define TEST_PIN 12

void app_main(void) {
    systime_get();

    esp_err_t err = ESP_OK;

    err = nvs_flash_init();  // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS flash memory
        err = nvs_flash_init();              // And try initialising it again
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(wifi_drv_init());          // Initialise WiFi
    while (wifi_drv_ip_assigned() == false) {  // Test whether IP addr has been assigned
    }

    systime_synchronise();
    systime_get();

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