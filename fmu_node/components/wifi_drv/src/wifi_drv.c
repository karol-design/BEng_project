/**
 * @file    wifi_drv.c
 * @brief   Initialize, configure and start Wi-Fi in a station mode, handle WiFi and IP events
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "wifi_drv.h"

#define TAG "wifi_drv"

/**
 * @brief WIFI and IP events handler
 */
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

/**
 * @brief Initialize, configure and start Wi-Fi in a station mode
 * @return Error code
 */
esp_err_t wifi_drv_init() {
    ESP_ERROR_CHECK(esp_netif_init());                 // Create an LwIP core task and initialize LwIP-related work
    ESP_ERROR_CHECK(esp_event_loop_create_default());  // Create an event loop to handle events from the WiFi task

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();  // Use default configuration parameters
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));                 // Initialize WiFi, allocate resource, start WiFi task
    ESP_LOGI(TAG, "WiFi initialised sucessfully");

    // Register an event handler for WIFI and IP events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    // Initialize default station as network interface instance (esp-netif)
    esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Define WiFi Station configuration
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .scan_method = WIFI_FAST_SCAN,             // Use fast scan, i.e. end after find SSID match AP
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,  // Sort AP by RSSI
            .threshold.rssi = (int8_t)(-127),          // Weakest RSSI to be considered
            .threshold.authmode = WIFI_AUTH_OPEN,      // Weakest authentication mode (no security)
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));                // Set mode to station
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));  // Set configuration
    ESP_LOGI(TAG, "WiFi configured sucessfully");
    ESP_ERROR_CHECK(esp_wifi_start());  // Start WiFi
    ESP_LOGI(TAG, "WiFi started sucessfully");

    return ESP_OK;
}