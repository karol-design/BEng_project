#include <stdio.h>
#include "esp_log.h"
#include "test_driver.h"

#define TAG "Main"

void app_main(void)
{
    ESP_LOGI(TAG, "This is a message from the main.c");
    ESP_LOGI(TAG, "Value returned by the driver function is %d", func());
}
