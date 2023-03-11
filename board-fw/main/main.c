/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "f_measurement.h"
#include "freertos/FreeRTOS.h"
#include "mqtt_drv.h"
#include "nvs_flash.h"
#include "systime.h"
#include "wifi_drv.h"

#define TAG "app"
#define SYS_SELF_TEST  // System self-test on/off macro

void app_main(void) {
    esp_err_t err = ESP_OK;
    struct timeval time;     // Struct to hold current system time
    mqtt_payload_t payload;  // MQTT Payload structure

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

    mqtt_drv_send();
    
    while (true) {
        ESP_LOGI(TAG, "Test...");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
