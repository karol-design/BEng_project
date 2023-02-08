/**
 * @file    wifi_drv.c
 * @brief   Initialize, configure and start Wi-Fi in a station mode, handle WiFi and IP events
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

esp_err_t wifi_drv_init();