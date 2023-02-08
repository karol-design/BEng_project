/**
 * @file    timer_drv.h
 * @brief   Initialise timer, reset and get current count value
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <stdio.h>

#include "driver/timer.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

esp_err_t drv_timer_init();
uint64_t drv_timer_get_count_isr();