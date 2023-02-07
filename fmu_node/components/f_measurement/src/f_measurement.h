/**
 * @file    f_measurement.h
 * @brief   Initialising interrupt, handling the ISR events via a seperate task, calculating the frequency
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include "esp_err.h"

esp_err_t f_measurement_init(uint64_t gpio_interrupt);
esp_err_t f_measurement_test(const uint64_t gpio_zco);
float f_measurement_get_val();