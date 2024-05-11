// standard includes for main
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

// webui.c
#include "webui.c"

static const char *TAG_main = "MAIN";

// ACD / moisture measurements
#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_0;     // GPIO4 if ADC1, GPIO14 if ADC2
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_2;
static uint32_t MOIST_MEASUREMENT = 0;

// void check_efuse() {
//     // Check TP is burned into eFuse
//     if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
//         printf("eFuse Two Point: Supported\n");
//     } else {
//         printf("eFuse Two Point: NOT supported\n");
//     }
//     // Check Vref is burned into eFuse
//     if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
//         printf("eFuse Vref: Supported\n");
//     } else {
//         printf("eFuse Vref: NOT supported\n");
//     }
// }

// void print_char_val_type(esp_adc_cal_value_t val_type) {
//     if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
//         printf("Characterized using Two Point Value\n");
//     } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
//         printf("Characterized using eFuse Vref\n");
//     } else {
//         printf("Characterized using Default Vref\n");
//     }
// }

void moisture_meter_task(void *pvParameters)
{
    /*
        ---
    */

    // Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    // Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    // Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;
        // Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, ADC_WIDTH_BIT_12, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        // Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        // printf("Raw: %ld\tVoltage: %ldmV\n", adc_reading, voltage);
        MOIST_MEASUREMENT = voltage;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void log_moist_value(void *pvParameters)
{
    /*
    */
    uint16_t *time_delay = (uint16_t *)pvParameters;
    while (1){
        if (*time_delay == 0){
            // vTaskDelay(pdMS_TO_TICKS(*time_delay));
            vTaskDelay(pdMS_TO_TICKS(3000));
            printf(" --- MAIN APP: Moisture Meter Voltage [mV] %ld with delay: %u\n", MOIST_MEASUREMENT, *time_delay);
        }
        else {
            vTaskDelay(pdMS_TO_TICKS(3000));
            printf(" --- MAIN APP: Moisture Meter Voltage [mV] %ld with delay: 1000\n", MOIST_MEASUREMENT);
        }
        
    }
}

    
// MAIN APP
void app_main() {

    run_webui();
    // xTaskCreate(run_webui, "Start web Interface", 4096, NULL, 5, NULL);

    //   TASKS MOISTURE METER
    xTaskCreate(moisture_meter_task, "moisture meter task", 2048, NULL, 1, NULL);
    uint16_t log_moist_value_delay = 1000;
    xTaskCreate(log_moist_value, "moist meter value", 4096, (void *)&log_moist_value_delay, 2, NULL);
}

