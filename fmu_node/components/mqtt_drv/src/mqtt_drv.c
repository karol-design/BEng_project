/**
 * @file    mqtt_drv.c
 * @brief   Define MQTT config details, initialise MQTT, set callback, start the client task and handle MQTT events
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "mqtt_drv.h"

#define TAG "mqtt_drv"

#define MQTT_URI "mqtt://mqtt3.thingspeak.com"    // ThingSpeak MQTT URI
#define MQTT_PORT 1883                            // TCP Port
#define MQTT_USERNAME "MxEJJyY4MwYHCS0TNzksJx4"   // Device Username
#define MQTT_PASSWORD "KBbyTJRU5/dlf+Fd+40Yu7pJ"  // Device Password
#define MQTT_ID "MxEJJyY4MwYHCS0TNzksJx4"         // Device ID

#define MQTT_FIELD_FREQ "channels/2033438/publish"  // Frequency/time channel topic

esp_mqtt_client_handle_t client;          // MQTT Client handle
static bool mqtt_connected_flag = false;  // Flag to indicate sucessfull connection to the MQTT broker

/**
 * @brief Event handler registered to receive MQTT events. This function is called by the MQTT client event loop.
 * @param handler_args user data registered to the event
 * @param base Event base for the handler(always MQTT Base in this example)
 * @param event_id The id for the received event
 * @param event_data The data for the event, esp_mqtt_event_handle_t
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_connected_flag = true;
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_connected_flag = false;
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("Topic = %.*s\r\n", event->topic_len, event->topic);
            printf("Data = %.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id: %d", event->event_id);
            break;
    }
}

bool mqtt_drv_connected() {
    return mqtt_connected_flag;
}

void mqtt_drv_send(float frequency, uint64_t time_ms, const char *str_status) {
    char message[100] = "field1=";

    char str_frequency[10];  // Declare char arrays for frequncy and time values
    char str_time[20];

    sprintf(str_frequency, "%.3lf", frequency);  // Convert float frequency to str
    sprintf(str_time, "%llu", time_ms);          // Convert llu int time_ms to str

    strcat(message, str_frequency);  // Concatenate strings to create a message
    strcat(message, "&field2=");
    strcat(message, str_time);
    strcat(message, "&status=");
    strcat(message, str_status);

    int msg_id = esp_mqtt_client_publish(client, MQTT_FIELD_FREQ, message, 0, 0, 0);
    ESP_LOGI(TAG, "Frequency, timestamp and status published successfully, msg_id = %d", msg_id);
}

/**
 * @brief Define MQTT config details, initialise MQTT, set callback and start the client
 * @return Error code
 */
esp_err_t mqtt_drv_init() {
    esp_err_t err = ESP_OK;

    // Define MQTT configuration details
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_URI,
        .port = MQTT_PORT,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD,
        .client_id = MQTT_ID,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);  // Initialise MQTT client

    if (client != NULL) {  // Check whether the returned MQTT handle is valid
        ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
        ESP_ERROR_CHECK(esp_mqtt_client_start(client));  // Start the MQTT client
        err = ESP_OK;
    } else {
        err = ESP_FAIL;
    }
    return err;
}
