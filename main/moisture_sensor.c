#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>


// ACD / moisture measurements valiables
#define DEFAULT_VREF    1100
#define NO_OF_SAMPLES   64          

static esp_adc_cal_characteristics_t *adc_chars;
static const adc2_channel_t channel = ADC_CHANNEL_0;     
static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_unit_t unit = ADC_UNIT_1;

static double MOISTURE_MEASUREMENT = 0.0;
static double max_voltage = 3145.0;


void moisture_meter_task(void *pvParameters)
{
    // adc config
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

    // read values
    while (1) {
        uint32_t adc_reading = 0;
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
        // convert values to voltage
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        MOISTURE_MEASUREMENT = (1 - voltage/max_voltage) * 100;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}