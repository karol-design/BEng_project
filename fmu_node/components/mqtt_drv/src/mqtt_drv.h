/**
 * @file    mqtt_drv.c
 * @brief   ...
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "mqtt_client.h"
// mqtt/esp-mqtt/include/mqtt_client.h

esp_err_t mqtt_drv_init();
