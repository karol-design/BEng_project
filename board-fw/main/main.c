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
#include "ws2812_drv.h"

#define TAG "app"

void app_main(void) {
    ESP_ERROR_CHECK(ws2812_drv_init());
    ESP_ERROR_CHECK(ws2812_drv_startup_animation(255));
    esp_err_t err = ESP_OK;
    mqtt_payload_t payload;  // MQTT Payload structure

    err = nvs_flash_init();  // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS flash memory
        err = nvs_flash_init();              // And try initialising it again
    }

    ESP_ERROR_CHECK(ws2812_drv_set_color(10, 10, 100, 60));
    ESP_ERROR_CHECK(wifi_drv_init());        // Initialise WiFi
    while (wifi_drv_connected() == false) {  // Wait for the device to connect to the AP
    }
    ESP_LOGI(TAG, "WiFi RSSI: %d", wifi_drv_get_rssi());
    ESP_ERROR_CHECK(ws2812_drv_set_color(10, 10, 100, 255));
    vTaskDelay(500 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(ws2812_drv_set_color(50, 100, 10, 60));
    ESP_ERROR_CHECK(systime_synchronise());  // Synchronise system time using SNTP
    systime_log();                           // Print synchronised time
    ESP_ERROR_CHECK(ws2812_drv_set_color(50, 100, 10, 255));
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(ws2812_drv_set_color(0, 0, 0, 255));

    ESP_ERROR_CHECK(mqtt_drv_init());       // Initialise MQTT
    while (mqtt_drv_connected() != true) {  // Wait for the device to connect to the MQTT broker
    }

    ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));  // Initialise frequency measurement

#ifdef SYS_SELF_TEST
    ESP_LOGW(TAG, "-------- Start frequency measurement test --------\n");
    ESP_ERROR_CHECK(f_measurement_test(TEST_PIN));  // Initialise ZCO simulation
#endif

    /**** Infinite measure - upload loop ****/
    int64_t n = 0;
    while (true) {
        if (wifi_drv_fault() == true || mqtt_drv_connected() == false) {
            esp_restart();  // Reboot the microcontroller
        }
        f_measurement_t meas = f_measurement_get_val();       // Read frequency and timestamp
        payload.d[n % MQTT_MEAS_PER_BURST].f_hz = meas.freq;  // Copy the frequency value to payload
        payload.d[n % MQTT_MEAS_PER_BURST].t_ms = meas.time;  // Copy the timestamp to payload

        if (payload.d[n % MQTT_MEAS_PER_BURST].f_hz != -1.0) {  // Check if a new value was available
            if (((n + 1) % MQTT_MEAS_PER_BURST) == 0) {         // Send MQTT_MEAS_PER_BURST new datapoints through MQTT
                ESP_LOGD(TAG, "Sending %d new data points to the MQTT queue", MQTT_MEAS_PER_BURST);
                ESP_ERROR_CHECK(mqtt_drv_queue_send(payload, sizeof(payload)));
                ESP_ERROR_CHECK(ws2812_drv_set_color(0, 250, 10, 255));
                vTaskDelay(60 / portTICK_PERIOD_MS);
                ESP_ERROR_CHECK(ws2812_drv_set_color(0, 0, 0, 255));
            }
            n++;  // Increment data point number
        }
    }
}
