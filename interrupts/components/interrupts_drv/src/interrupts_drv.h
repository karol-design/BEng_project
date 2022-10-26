/**
 * @file    interrupts_drv.h
 * @brief   Initialising interrupt and ISR, handling the ISR events via a seperate task
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include "esp_err.h"

esp_err_t interrupt_init();