/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_check.h"
#include "f_measurement.h"

#define TAG "App"

#define ZCO_PIN 4

void app_main(void) {
    ESP_LOGI(TAG, "Frequency measurement test");
    ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));
    ESP_ERROR_CHECK(f_measurement_test());
}