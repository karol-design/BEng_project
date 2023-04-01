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
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
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
                ESP_LOGE(TAG, "Last error string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
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
        ESP_LOGD(TAG, "Data sucessfully sent to mqtt queue");
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

/**
 * @brief Send MQTT message with frequency, time and status update
 * @param data MQTT payload structure with an array of datapoints (f_hz and t_ms)
 * @param str_status Status of the device
 */
static void mqtt_drv_send(mqtt_payload_t data, const char *str_status) {
    char message[MQTT_MESSAGE_SIZE] = "field1=";

    for (int i = 0; i < MQTT_MEAS_PER_BURST; i++) {
        data.d[i].t_ms -= 1600000000000;  // Decrement and divide ms data to facilitate more date per MQTT message
        data.d[i].t_ms /= 100;
    }

    char str_frequency[MQTT_MEAS_PER_BURST][10];  // Declare two arrays of strings for frequency...
    char str_time[MQTT_MEAS_PER_BURST][20];       // ... and time values

    for (int i = 0; i < MQTT_MEAS_PER_BURST; i++) {
        sprintf(str_frequency[i], "%.3lf", data.d[i].f_hz);  // Convert float frequency to str
        sprintf(str_time[i], "%llu", data.d[i].t_ms);        // Convert llu int time_ms to str
    }

    for (int i = 0; i < MQTT_MEAS_PER_BURST; i++) {
        strcat(message, str_frequency[i]);  // Concatenate strings to create a message
        strcat(message, ",");               // Add a coma to format datapoints as a csv packet
    }

    strcat(message, "&field2=");
    for (int i = 0; i < MQTT_MEAS_PER_BURST; i++) {
        strcat(message, str_time[i]);  // Concatenate strings to create a message
        strcat(message, ",");
    }

    char str_no_datapoints[5];                              // String to hold no of datapoints in the MQTT message
    sprintf(str_no_datapoints, "%d", MQTT_MEAS_PER_BURST);  // Convert no of datapoints to str
    strcat(message, "&field3=");
    strcat(message, str_no_datapoints);

    strcat(message, "&status=");
    strcat(message, str_status);

    int msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC, message, 0, 0, 0);
}

/**
 * @brief Task for reading values from the data que and sending them in bursts of 10 through MQTT
 */
static void mqtt_drv_task(void *param) {
    mqtt_payload_t data;               // Struct with the data to be sent
    static uint64_t upload_count = 1;  // Upload counter variable
    char status[40];
    while (1) {
        if (xQueueReceive(mqtt_queue, &data, (TickType_t)0) == pdTRUE) {    // Check if a pointer to a new data set is available (no blocking)
            // Format device status string
            sprintf(status, "Device OK, No. %03llu, MPB: %d, MPS: %d", upload_count++, MQTT_MEAS_PER_BURST, (50/PULSES_PER_MEAS)); 
            mqtt_drv_send(data, status);
            ESP_LOGI(TAG, "Datapoint succesfully published, no. %03llu", (upload_count-1));
        }
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

    client = esp_mqtt_client_init(&mqtt_cfg);  // Initialise MQTT client

    if (client != NULL) {  // Check whether the returned MQTT handle is valid
        ESP_RETURN_ON_ERROR(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL), TAG, "Failed to register MQTT Event handler");
        ESP_RETURN_ON_ERROR(esp_mqtt_client_start(client), TAG, "Failed to start the MQTT Client");  // Start the MQTT client
        err = ESP_OK;
    } else {
        err = ESP_FAIL;
        ESP_RETURN_ON_ERROR(err, TAG, "Failed to initialise MQTT client (NULL pointer returned)");
    }

    mqtt_queue = xQueueCreate(1, sizeof(mqtt_payload_t));          // Create a queue for data to be sent
    xTaskCreate(mqtt_drv_task, "MQTT_TASK", 8192, NULL, 10, NULL);  // Create and start the MQTT task
    ESP_LOGI(TAG, "MQTT task initialised");

    return err;
}
