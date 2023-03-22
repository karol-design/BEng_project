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
    esp_err_t err = ESP_OK;

    err = nvs_flash_init();  // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS flash memory
        err = nvs_flash_init();              // And try initialising it again
    }

    ESP_ERROR_CHECK(ws2812_drv_init());  // Initialise WS2812 LED Driver
    ESP_ERROR_CHECK(ws2812_drv_startup_animation(255));  // Run Startup animation

    while (1) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        ws2812_drv_set_color(255,10,10,80);  
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        ws2812_drv_set_color(10,255,10,80);  
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        ws2812_drv_set_color(10,10,255,80);  
    }
}
