/**
 * @file    systime.c
 * @brief   Synchronise time using LwIP SNTP, get current system time
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "systime.h"

#define TAG "systime"
#define SNTP_SYNCH_RETRY 10    // Max number of SNTP synchronisation attempts
#define SNTP_SYNCH_DELAY 1000  // Delay in ms between first and successive attempts to synch. time

/**
 * @brief Initialise SNTP
 */
static void initialize_sntp() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);  // Set the operation mode to poll
    sntp_setservername(0, "pool.ntp.org");    // Set the address of the NTP server
    sntp_init();
}

/**
 * @brief Synchronise system time using SNTP
 * @return Error code
 */
esp_err_t systime_synchronise() {
    initialize_sntp();  // Initialise SNTP and begin time synchronisation
    int retry = 0;      // Synchronisation attempts count
    vTaskDelay(SNTP_SYNCH_DELAY / portTICK_PERIOD_MS);
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < SNTP_SYNCH_RETRY) {
        ESP_LOGW(TAG, "Reattempting SNTP time synchronisation... (%d/%d)", retry, SNTP_SYNCH_RETRY);
        vTaskDelay(SNTP_SYNCH_DELAY / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "System time synchronised successfully");
    return ESP_OK;
}

/**
 * @brief Get current system time
 * @return Time value structure
 */
struct timeval systime_get() {
    time_t now;          // Number of seconds (int) since 00:00, Jan 1 1970 UTC (i.e. POSIX time)
    struct tm timeinfo;  // Time structure
    time(&now);          // Copy the current system time to the variable now
    char strftime_buf[64];

    setenv("TZ", "GMTGMT-1,M3.4.0/01,M10.4.0/02", 1);  // Set environmental var TZ to UK timezone
    tzset();                                           // Apply the timezone (TZ env variable value)
    localtime_r(&now, &timeinfo);                      // Convert time into calendar time (local time) in the struct tm format

    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);  // Convert the time structure info to string
    ESP_LOGI(TAG, "The current date/time (PL): %s", strftime_buf);

    struct timeval sys_time;        // Time value structure (with s and us members)
    gettimeofday(&sys_time, NULL);  // Copy current sys time to the structure object
    ESP_LOGI(TAG, "The current time is: %llu s and %llu us", (int64_t)sys_time.tv_sec, (int64_t)sys_time.tv_usec);

    return sys_time;
}