/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include <sys/time.h>

#include "esp_check.h"
#include "f_measurement.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "systime.h"
#include "wifi_drv.h"
#include "mqtt_drv.h"

#define TAG "app"
#define ZCO_PIN 4
#define TEST_PIN 12

void app_main(void) {
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    esp_log_level_set("*", ESP_LOG_INFO);

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

    ESP_ERROR_CHECK(systime_synchronise());  // Synchronise system time using SNTP
    // struct timeval time;                     // Create a structure to hold current time data

    // ESP_LOGI(TAG, "Frequency measurement test");
    // ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));   // Initialise frequency measurement
    // ESP_ERROR_CHECK(f_measurement_test(TEST_PIN));  // Initialise ZCO simulation & freq. measurement test

    // while (true) {
    //     float freq = f_measurement_get_val();  // Read frequency
    //     if (freq != -1.0) {                    // Check if a new value was available
    //         gettimeofday(&time, NULL);         // Copy current sys time to sys_time struct
    //         ESP_LOGI(TAG, "Freq: %lf Hz | Time: %llu s %llu us", freq, (int64_t)time.tv_sec, (int64_t)time.tv_usec);
    //     }
    // }

    ESP_ERROR_CHECK(mqtt_drv_init());
}