/**
 * @file    f_measurement.h
 * @brief   Initialising interrupt, handling the ISR events via a seperate task, calculating the frequency
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "config_macros.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

typedef struct measurement {  // Single measurement datatype
    float freq;
    uint64_t time;
} f_measurement_t;

esp_err_t f_measurement_init(uint64_t gpio_interrupt);
esp_err_t f_measurement_test(const uint64_t gpio_zco);
f_measurement_t f_measurement_get_val();