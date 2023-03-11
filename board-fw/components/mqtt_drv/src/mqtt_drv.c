/**
 * @file    mqtt_drv.c
 * @brief   Define MQTT config details, initialise MQTT, set callback, start the client task and handle MQTT events
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "mqtt_drv.h"

#define TAG "mqtt_drv"

esp_mqtt_client_handle_t client;          // MQTT Client handle
static bool mqtt_connected_flag = false;  // Flag to indicate sucessfull connection to the MQTT broker
static xQueueHandle mqtt_queue = NULL;    // Queu for data to be sent

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

/**
 * @brief Getter function for checking if the device is connected to the MQTT broker
 * @return True if connected to broker, False otherwise
 */
bool mqtt_drv_connected() {
    return mqtt_connected_flag;
}

/**
 * @brief Send a new array of data structs to the MQTT que for data to be send
 * @param ready_data Structure with datapoints to be sent
 * @param data_size Size of the structure
 * @return Error code
 */
esp_err_t mqtt_drv_queue_send(mqtt_payload_t ready_data, size_t data_size) {
    if (xQueueSend(mqtt_queue, &ready_data, (TickType_t)0) == pdTRUE) {  // Send a new struct with an array of datapoints to the que
        ESP_LOGI(TAG, "Data sucessfully sent to mqtt queue");
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

/**
 * @brief Task for reading values from the data que and sending them in bursts of 10 through MQTT
 */
static void mqtt_drv_task(void *param) {
    mqtt_payload_t data;  // Struct with the data to be sent
    while (1) {
        if (xQueueReceive(mqtt_queue, &data, (TickType_t)0) == pdTRUE) {  // Check if a pointer to a new data set is available (no blocking)
            for (int i = 0; i < MQTT_MEAS_PER_BURST; i++) {               // Go through all data points and send them one-by-one
                mqtt_drv_send(data.d[i].f_hz, data.d[i].t_ms, data.d[i].dev_stat);
                vTaskDelay(MQTT_DELAY_BETWEEN_MESS / portTICK_PERIOD_MS);  // Wait for a set amount of ms before sending the next message
            }
        }
    }
}

/**
 * @brief Send MQTT message with frequency, time and status update
 * @param frequency Measured frequency [Hz]
 * @param time_ms UNIX timestamp of the measurement [ms]
 * @param str_status Status of the device
 */
static void mqtt_drv_send(float frequency, uint64_t time_ms, const char *str_status) {
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

    int msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC, message, 0, 0, 0);
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

    mqtt_queue = xQueueCreate(1, sizeof(mqtt_payload_t));  // Create a queue for data to be sent
    xTaskCreate(mqtt_drv_task, "MQTT_TASK", 8192, NULL, 10, NULL);  // Create and start the MQTT task
    ESP_LOGI(TAG, "MQTT task initialised");

    return err;
}
