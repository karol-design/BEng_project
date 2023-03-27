/**
 * @file    timer_drv.c
 * @brief   Initialise timer, reset and get current count value
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "timer_drv.h"

#define TAG "timer_drv"

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
    timer_config_t config = {
        // Select and initialize basic parameters of the timer
        .divider = TIMER_DIVIDER,  // Clock source is APB. Run the timer at 40 MHz (max available freq.)
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,            // Start counting when initialised
        .alarm_en = TIMER_ALARM_DIS,          // No alarms
        .auto_reload = TIMER_AUTORELOAD_DIS,  // No interrupts
    };

    // Initialise the timer
    ESP_RETURN_ON_ERROR(timer_init(TIMER_GROUP, TIMER_NUM, &config), TAG, "Failed to initialise the timer");

    static const uint64_t initial_count = 0;
    ESP_RETURN_ON_ERROR(timer_set_counter_value(TIMER_GROUP, TIMER_NUM, initial_count), TAG, "Failed to set the initial timer count to 0");

    ESP_LOGI(TAG, "Timer initialised and running");
    return ESP_OK;
}
