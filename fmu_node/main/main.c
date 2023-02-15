/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "esp_check.h"
#include "f_measurement.h"
#include "freertos/FreeRTOS.h"
#include "mqtt_drv.h"
#include "nvs_flash.h"
#include "systime.h"
#include "wifi_drv.h"

#define TAG "app"
#define ZCO_PIN 4
#define TEST_PIN 12

void app_main(void) {
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_err_t err = ESP_OK;
    struct timeval time;

    err = nvs_flash_init();  // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS flash memory
        err = nvs_flash_init();              // And try initialising it again
    }

    ESP_ERROR_CHECK(wifi_drv_init());          // Initialise WiFi
    while (wifi_drv_ip_assigned() == false) {  // Wait for the device to get an IP addr
    }

    ESP_ERROR_CHECK(systime_synchronise());  // Synchronise system time using SNTP
    systime_log();                           // Print synchronised time

    ESP_ERROR_CHECK(mqtt_drv_init());       // Initialise MQTT
    while (mqtt_drv_connected() != true) {  // Wait for the device to connect to the MQTT broker
    }

    ESP_LOGW(TAG, "-------- Start frequency measurement test --------\n");
    ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));   // Initialise frequency measurement
    ESP_ERROR_CHECK(f_measurement_test(TEST_PIN));  // Initialise ZCO simulation

    /**** Infinite measure - upload loop ****/
    while (true) {
        float freq = f_measurement_get_val();                                                       // Read frequency
        if (freq != -1.0) {                                                                         // Check if a new value was available
            gettimeofday(&time, NULL);                                                              // Copy current sys time to sys_time struct
            uint64_t time_ms = (((uint64_t)time.tv_sec * 1000) + ((uint64_t)time.tv_usec / 1000));  // Calculate time in ms
            ESP_LOGI(TAG, "| Freq: %.3lf Hz | Time: %llu s %llu us | UNIX Time: %llu", freq, (int64_t)time.tv_sec, (int64_t)time.tv_usec, time_ms);
            char status[50];  // String to hold device status
            sprintf(status, "Chip: %s | Free mem: %d bytes", CONFIG_IDF_TARGET, esp_get_free_heap_size());
            mqtt_drv_send(freq, time_ms, status);  // Send frequency, time and status
        }
    }
}
