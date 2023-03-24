/**
 * @file    ws2812_drv.c
 * @brief   Initialize and control WS2812 RGB LED
 * @note    Based on https://github.com/JSchaenzle/ESP32-NeoPixel-WS2812-RMT
 * @author  Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)
 */

#include "ws2812_drv.h"

#define TAG "ws2812_drv"

static rmt_item32_t rmt_data_buffer[WS2812_BITS_PER_CMD];  // Data buffet for the RMT peripheral

/**
 * @brief Configure and initialise RMT peripheral required for communicating with WS2812
 * @return Error code
 */
esp_err_t ws2812_drv_init(void) {
    rmt_config_t config = {
        .rmt_mode = RMT_MODE_TX,        // Set the RMT in transmitter mode
        .channel = WS2812_RMT_CHANNEL,  // Configure the channel RMT should use for WS2812
        .gpio_num = WS2812_PIN,         // Set the appropriate GPIO pin
        .mem_block_num = 3,
        .tx_config.loop_en = false,
        .tx_config.carrier_en = false,
        .tx_config.idle_output_en = true,
        .tx_config.idle_level = 0,
        .clk_div = 2};

    ESP_ERROR_CHECK(rmt_config(&config));                       // Configure RMT based on the config struct
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));  // Initialise RMT driver
    ESP_LOGI(TAG, "RMT (WS2812) driver initialised and running");

    return ESP_OK;
}

/**
 * @brief Populate the RMT buffer according to the new LED state
 * @param led_state 32-bit uint value representing new LED state
 */
static void ws2812_drv_set_rmt_buffer(led_state_t led_state) {
    uint32_t bit_mask = 1 << (WS2812_BITS_PER_CMD - 1);  // Prepare bit mask for testing subsequent bits

    for (uint32_t bit = 0; bit < WS2812_BITS_PER_CMD; bit++) {  // Set bits in the buffer one by one
        uint32_t bit_set = led_state & bit_mask;                // Test if the bit of interest is set in the led_state

        // Depending on the bit value (1/0) use appropriate structs for the RMT buffer
        rmt_data_buffer[bit] = bit_set ? (rmt_item32_t){{{WS2812_T1H, 1, WS2812_T1L, 0}}} : (rmt_item32_t){{{WS2812_T0H, 1, WS2812_T0L, 0}}};
        bit_mask >>= 1;  // Advance in the testing bit mask
    }
}

/**
 * @brief Update the RMT buffer, write the data and wait for the transmission to end
 * @param led_state 32-bit uint value representing new LED state
 * @return Error code
 */
static esp_err_t ws2812_drv_update(led_state_t led_state) {
    ws2812_drv_set_rmt_buffer(led_state);  // Populate the RMT buffer according to the new LED state

    // Write the bits from the buffer and block until the transmission is finished
    ESP_ERROR_CHECK(rmt_write_items(WS2812_RMT_CHANNEL, rmt_data_buffer, WS2812_BITS_PER_CMD, false));
    ESP_ERROR_CHECK(rmt_wait_tx_done(WS2812_RMT_CHANNEL, portMAX_DELAY));

    return ESP_OK;
}

/**
 * @brief Set WS2812 color
 * @param r Red color intensity as 8-bit uint value
 * @param g Green color intensity as 8-bit uint value
 * @param b Blue color intensity as 8-bit uint value
 * @param brightness Brightness [0-255]
 * @return Error code
 */
esp_err_t ws2812_drv_set_color(uint32_t r, uint32_t g, uint32_t b, uint32_t brightness) {
    // Calculate intensities for each color considering overall brightness
    r = (r * brightness) / 255;
    g = (g * brightness) / 255;
    b = (b * brightness) / 255;

    r <<= 16;  // Red value is stored as bits 24 to 17
    g <<= 8;   // Green intensity is stored in bits 16 to 9

    led_state_t led_state = (((r | g) | b) & (0x00FFFFFF));  // Combine r, g and b values into one uint
    ESP_ERROR_CHECK(ws2812_drv_update(led_state));           // Update the LED

    return ESP_OK;
}

/**
 * @brief Run startup RG animation using WS2812
 * @param brightness Brightness [0-255]
 * @return Error code
 */
esp_err_t ws2812_drv_startup_animation(uint32_t brightness) {
    ESP_LOGI(TAG, "Run startup animation");
    for (uint32_t i = 0; i < 100; i++) {  // Increase blue color intensity
        ESP_ERROR_CHECK(ws2812_drv_set_color(i, 0, i, brightness));
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    for (uint32_t i = 0; i < 75; i++) {  // Keep increasing blue intens. and start increasing green intens.
        ESP_ERROR_CHECK(ws2812_drv_set_color(100, i, 100 + i, brightness));
        vTaskDelay(15 / portTICK_PERIOD_MS);
    }

    for (uint32_t i = 0; i < 70; i++) {  // Mix all three colors slowly increasing their intensity
        ESP_ERROR_CHECK(ws2812_drv_set_color(100 + (i * 2), 75 + (i * 2), 175 + i, brightness));
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    for (uint32_t i = brightness; i > 0; i--) {  // Mix all three colors slowly increasing their intensity
        ESP_ERROR_CHECK(ws2812_drv_set_color(240, 215, 245, i));
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    vTaskDelay(100 / portTICK_PERIOD_MS);

    return ESP_OK;
}
