/**
 * @file    main.c
 * @brief   Main application file
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_check.h"
#include "timers_drv.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "App"

#define PULSES_PER_MEASUREMENT 10
#define MEASUREMENT_FINISHED 0x01   // Measurement finished flag

volatile bool timer_en = true;
volatile uint32_t pulses = 0;

/* Pulses 1-10 (measure), 11-20 (break), 21...*/
void isr() {
    if(pulses%PULSES_PER_MEASUREMENT == 0) {    // Every PULSES_PER_MEASUREMENT pulses...
        drv_timer_enable_isr(timer_en);         // ...enable or disable the timer.
        if(timer_en == false) {                                 // If the timer has been just disabled...
            uint64_t count = drv_timer_get_count_isr();         // ...get the count...
            //xQueueSendFromISR(gpio_evt_queue, &count, NULL);    // ...send it to the queue...
            timer_en = true;                                    // ...and set timer_en flag for next measurement
        } else {                // If the timer has been just disabled...
            timer_en = false;   // ...reset timer_en flag, to stop counting after PULSES_PER_MEASUREMENT pulses.
        }
    }
    pulses++;   // Increment pulses count each time ISR is executed
}

void app_main(void) {

    ESP_LOGI(TAG, "Timer test");
    ESP_ERROR_CHECK(drv_timer_init());

    while(true) {
        ESP_LOGI(TAG, "Count = %llu", drv_timer_get_count_isr());
        drv_timer_reset();
        isr();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}