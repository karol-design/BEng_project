/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>

#include "esp_check.h"
#include "esp_log.h"
#include "f_measurement.h"

#define TAG "app"

#define ZCO_PIN 4
#define TEST_PIN 12

void app_main(void) {
    ESP_LOGI(TAG, "Frequency measurement test");
    ESP_ERROR_CHECK(f_measurement_init(ZCO_PIN));   // Initialise frequency measurement
    ESP_ERROR_CHECK(f_measurement_test(TEST_PIN));  // Initialise ZCO simulation & freq. measurement test

    while (true) {
        float freq = f_measurement_get_val();  // Read measured frequency if a new value is available
    }
}