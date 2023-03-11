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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "mqtt_client.h"

typedef struct datapoint {  // Single datapoint data type
    float f_hz;             // Frequency in Hz
    uint64_t t_ms;          // Timestamp in ms as Unix time
    char dev_stat[8];       // Device status
} mqtt_datapoint_t;

typedef struct payload {                      // MQTT payload wrapper data type
    mqtt_datapoint_t d[MQTT_MEAS_PER_BURST];  // An array of MQTT_MEAS_PER_BURST datapoints
} mqtt_payload_t;

esp_err_t mqtt_drv_init();
esp_err_t mqtt_drv_queue_send(mqtt_payload_t ready_data, size_t data_size);
static void mqtt_drv_task(void *param);
void mqtt_drv_send();
bool mqtt_drv_connected();
