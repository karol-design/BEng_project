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

#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

#define TAG "app"

void app_main(void) {
    esp_err_t err = ESP_OK;
    struct led_state led_state_s;

    err = nvs_flash_init();  // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Erase NVS flash memory
        err = nvs_flash_init();              // And try initialising it again
    }

    ws2812_control_init();

    while (1) {
        led_state_s.leds[0] = RED;
        ws2812_write_leds(led_state_s);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        led_state_s.leds[0] = GREEN;
        ws2812_write_leds(led_state_s);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        led_state_s.leds[0] = BLUE;
        ws2812_write_leds(led_state_s);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
