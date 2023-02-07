/**
 * @file    timers_drv.h
 * @brief   Initialising timers
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include "esp_err.h"

esp_err_t drv_timer_init();
uint64_t drv_timer_get_count_isr();