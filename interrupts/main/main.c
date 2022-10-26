#include <stdio.h>
#include "esp_log.h"    // Logging macro
#include "esp_check.h"  // Error checking macro
#include "interrupts_drv.h"

#define TAG "App"

void app_main(void)
{
    ESP_LOGI(TAG, "Interrupt test...");
    ESP_ERROR_CHECK(interrupt_init());
}
