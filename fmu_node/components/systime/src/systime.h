/**
 * @file    systime.c
 * @brief   Synchronise time using LwIP SNTP, get current system time
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"

esp_err_t systime_synchronise();
struct timeval systime_log();