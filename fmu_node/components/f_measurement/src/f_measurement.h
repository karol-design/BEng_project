/**
 * @file    f_measurement.h
 * @brief   Initialising interrupt, handling the ISR events via a seperate task, calculating the frequency
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "timer_drv.h"

esp_err_t f_measurement_init(uint64_t gpio_interrupt);
esp_err_t f_measurement_test(const uint64_t gpio_zco);
float f_measurement_get_val();