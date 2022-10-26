/**
 * @file        common_macros.h
 * @brief       Common macros for use throughout the project
 * @date        2022-10-26
 */

#pragma once

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"

#define INTERNAL_TAG "Debug"
	
#define VERIFY_SUCCESS(_err_code, ...)															\
	if ((_err_code) != ESP_OK) {																\
		ESP_LOGE(INTERNAL_TAG, "Error, code:%d %s", (_err_code), esp_err_to_name(_err_code));	\
		return _err_code;																		\
	}