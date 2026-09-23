#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "SENSOR_TASK";

void sensor_task(void *pvParameters) {
    // Initialize ADC1 unit
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    // Configure Channel 6 (GPIO 34)
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &config);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(2000); // 2000 ms period

    for (;;) {
        int raw_adc = 0;
        // Read raw ADC value from GPIO 34 (ADC1 Channel 6)
        adc_oneshot_read(adc1_handle, ADC_CHANNEL_6, &raw_adc);

        // Convert 12-bit raw value (0 - 4095) to light percentage (0 - 100%)
        float light_percent = ((float)raw_adc / 4095.0f) * 100.0f;

        // Baseline DHT22 values
        float temperature = 24.5f;
        float humidity = 55.0f;

        ESP_LOGI(TAG, "LDR Raw: %d | Light: %.1f%% | Temp: %.1f C | Humidity: %.1f%%",
                 raw_adc, light_percent, temperature, humidity);

        // Absolute timing delay to prevent drift
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "BCA152 Multisensor - Part IV Starting...");

    // Create SensorTask with Priority 2
    xTaskCreate(sensor_task, "SensorTask", 3072, NULL, 2, NULL);
}