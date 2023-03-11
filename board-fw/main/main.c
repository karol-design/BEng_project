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

    ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));  // Initialise frequency measurement

#ifdef SYS_SELF_TEST
    ESP_LOGW(TAG, "-------- Start frequency measurement test --------\n");
    ESP_ERROR_CHECK(f_measurement_test(TEST_PIN));  // Initialise ZCO simulation
#endif

    /**** Infinite measure - upload loop ****/
    int64_t n = 0;
    while (true) {
        payload.d[n % MQTT_MEAS_PER_BURST].f_hz = f_measurement_get_val();  // Read frequency
        if (payload.d[n % MQTT_MEAS_PER_BURST].f_hz != -1.0) {              // Check if a new value was available
            gettimeofday(&time, NULL);                                      // Copy current sys time to sys_time struct and calculate time in ms
            payload.d[n % MQTT_MEAS_PER_BURST].t_ms = (((uint64_t)time.tv_sec * 1000) + ((uint64_t)time.tv_usec / 1000));
            ESP_LOGI(TAG, "| Freq: %.3lf Hz | UNIX Time: %llu ms", payload.d[n % MQTT_MEAS_PER_BURST].f_hz, payload.d[n % MQTT_MEAS_PER_BURST].t_ms);
            sprintf(payload.d[n % 10].dev_stat, "OK 0x0");

            if (((n + 1) % MQTT_MEAS_PER_BURST) == 0) {  // Send MQTT_MEAS_PER_BURST new datapoints through MQTT
                ESP_LOGI(TAG, "Sending %d new data points to the MQTT queue", MQTT_MEAS_PER_BURST);
                ESP_ERROR_CHECK(mqtt_drv_queue_send(payload, sizeof(payload)));
            }
            n++;  // Increment data point number
        }
    }
}
