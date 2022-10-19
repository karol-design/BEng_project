#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define BLINK_GPIO 2

void app_main(void)
{
    static uint8_t s_led_state = 0;                     // Variable with LED state (default 0)
    gpio_reset_pin(BLINK_GPIO);                         // Set the GPIO as a push/pull output
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);   // Set the GPIO as an output

    while (1) {
        ESP_LOGI("Test", "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        gpio_set_level(BLINK_GPIO, s_led_state);    // Set the GPIO level according to the state
        s_led_state = !s_led_state;                 // Toggle the LED state
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS); // Non blocking delay
    }
}
