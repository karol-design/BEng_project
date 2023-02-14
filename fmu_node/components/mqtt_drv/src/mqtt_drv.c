/**
 * @file    mqtt_drv.c
 * @brief   ...
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "mqtt_drv.h"

#define TAG "mqtt_drv"

// Configuration for MQTT over TCP, port 1883, with username and password and device ID
#define MQTT_URI "mqtt://mqtt3.thingspeak.com" //mqtt://acme.thinger.io"
#define MQTT_PORT 1883
#define MQTT_USERNAME "MxEJJyY4MwYHCS0TNzksJx4"
#define MQTT_PASSWORD "KBbyTJRU5/dlf+Fd+40Yu7pJ"
#define MQTT_ID "MxEJJyY4MwYHCS0TNzksJx4"

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/**
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "channels/2033438/publish/fields/field1", "50.057", 0, 0, 0);
            ESP_LOGI(TAG, "Sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
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

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);  // Initialise MQTT client

    if (client != NULL) {  // Check whether the returned MQTT handle is valid
        ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
        ESP_ERROR_CHECK(esp_mqtt_client_start(client)); // Start the MQTT client
        err = ESP_OK;
    } else {
        err = ESP_FAIL;
    }
    return err;
}
