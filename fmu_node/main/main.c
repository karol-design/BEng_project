/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_check.h"
#include "interrupts_drv.h"

#define TAG "App"

void app_main(void) {
    ESP_LOGI(TAG, "Frequency measurement test");
    ESP_ERROR_CHECK(f_meaurement_init());
}