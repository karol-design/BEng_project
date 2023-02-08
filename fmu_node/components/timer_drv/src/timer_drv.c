/**
 * @file    timer_drv.c
 * @brief   Initialise timer, reset and get current count value
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "timer_drv.h"

#define TAG "timer_drv"

#define TIMER_DIVIDER (2)  //  Hardware timer clock divider (80/2 = 40 MHz)
#define TIMER_GROUP TIMER_GROUP_0
#define TIMER_NUM TIMER_0

/**
 * @brief Timer get count (to be used in ISRs)
 * @return Count
 */
uint64_t IRAM_ATTR drv_timer_get_count_isr() {
    uint64_t count = timer_group_get_counter_value_in_isr(TIMER_GROUP, TIMER_NUM);
    return count;
}

/**
 * @brief Timer initialisation
 * @return Error code
 */
esp_err_t drv_timer_init() {
    esp_err_t err = ESP_OK;

    timer_config_t config = {
        // Select and initialize basic parameters of the timer
        .divider = TIMER_DIVIDER,  // Clock source is APB. Run the timer at 40 MHz (max available freq.)
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,            // Start counting when initialised
        .alarm_en = TIMER_ALARM_DIS,          // No alarms
        .auto_reload = TIMER_AUTORELOAD_DIS,  // No interrupts
    };

    err = timer_init(TIMER_GROUP, TIMER_NUM, &config);  // Initialise the timer
    ESP_ERROR_CHECK(err);

    static const uint64_t initial_count = 0;
    err = timer_set_counter_value(TIMER_GROUP, TIMER_NUM, initial_count);
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "Timer initialised and running");
    return ESP_OK;
}