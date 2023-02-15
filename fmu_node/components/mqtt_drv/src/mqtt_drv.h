/**
 * @file    mqtt_drv.c
 * @brief   Define MQTT config details, initialise MQTT, set callback, start the client task and handle MQTT events
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config_macros.h"
#include "esp_event.h"
#include "esp_system.h"
#include "mqtt_client.h"

esp_err_t mqtt_drv_init();
void mqtt_drv_send(float frequency, uint64_t time_ms, const char *str_status);
bool mqtt_drv_connected();
