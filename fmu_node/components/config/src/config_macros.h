/**
 * @file    config_macros.h
 * @brief   Configuration macros and common includes for use throughout the project
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#pragma once

#include <string.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

/* PIN Assignment */
#define ZCO_PIN 4
#define TEST_PIN 12

/* WiFi */
#define WIFI_SSID "iPhone (Karol)"
#define WIFI_PASS "karol1234"

/* Timer */
#define TIMER_DIVIDER (2)  //  Hardware timer clock divider (80/2 = 40 MHz)
#define TIMER_GROUP TIMER_GROUP_0
#define TIMER_NUM TIMER_0

/* SNTP */
#define SNTP_SYNCH_RETRY 10    // Max number of SNTP synchronisation attempts
#define SNTP_SYNCH_DELAY 1000  // Delay in ms between first and successive attempts to synch. time

/* MQTT */
#define MQTT_URI "mqtt://mqtt3.thingspeak.com"      // ThingSpeak MQTT URI
#define MQTT_PORT 1883                              // TCP Port
#define MQTT_USERNAME "MxEJJyY4MwYHCS0TNzksJx4"     // Device Username
#define MQTT_PASSWORD "KBbyTJRU5/dlf+Fd+40Yu7pJ"    // Device Password
#define MQTT_ID "MxEJJyY4MwYHCS0TNzksJx4"           // Device ID
#define MQTT_FIELD_FREQ "channels/2033438/publish"  // Frequency/time channel topic
#define MQTT_MEAS_PER_BURST 5                       // Number of measurement per one burst MQTT upload
#define MQTT_DELAY_BETWEEN_MESS 1000                // Number of ms between adjacent MQTT messages

/* Frequency measurement */
#define ESP_INTR_FLAG_DEFAULT 0
#define PULSES_PER_MEAS 200  // Number of interrupt pulses for one frequency measurement