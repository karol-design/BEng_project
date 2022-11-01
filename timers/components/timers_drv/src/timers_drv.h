/**
 * @file    timers_drv.h
 * @brief   Initialising timers
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include "esp_err.h"

esp_err_t drv_timer_init();
esp_err_t drv_timer_reset();
uint64_t drv_timer_get_count();

void drv_timer_enable_isr(bool timer_en);
uint64_t drv_timer_get_count_isr();
