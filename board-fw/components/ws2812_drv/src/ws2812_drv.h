/**
 * @file    ws2812_drv.h
 * @brief   Initialize and control WS2812 RGB LED
 * @note    Based on https://github.com/JSchaenzle/ESP32-NeoPixel-WS2812-RMT
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <stdint.h>

#include "config_macros.h"
#include "driver/rmt.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

// 32-bit uint LED State type [from LSB: Byte 0 (Blue); Byte 1 (Green); Byte 2 (Red); Byte 3 (not used)]
typedef uint32_t led_state_t;

esp_err_t ws2812_drv_init(void);
esp_err_t ws2812_drv_set_color(uint32_t r, uint32_t g, uint32_t b, uint32_t brightness);
esp_err_t ws2812_drv_startup_animation(uint32_t brightness);
