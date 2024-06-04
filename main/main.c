// standard includes for main
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "driver/i2c.h"

// webui
#include "webui.c"

// light sensor
// #include "light_sensor.c"

static const char *TAG_main = "MAIN";

// ACD / moisture measurements
#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc2_channel_t channel = ADC_CHANNEL_0;     
static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_unit_t unit = ADC_UNIT_1;


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
        // konwersja 
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        // printf("Raw: %ld\tVoltage: %ldmV\n", adc_reading, voltage);
        MOISTURE_MEASUREMENT = voltage;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



    
// MAIN APP
void app_main() {
    //   xTask for moist meter
    xTaskCreate(moisture_meter_task, "moisture meter task", 4096, NULL, 2, NULL);

    //   xTask for solar meter
    // Initialize I2C master
    i2c_master_init();
    // Start task to read photodiode value
    xTaskCreate(&read_light_sensor_task, "read_light_sensor_task", 4096, NULL, 5, NULL);

    //   xTask for webui 
    xTaskCreate(run_webui, "Start web Interface", 4096, NULL, 5, NULL);

    while (1){
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("Moisture: %ld mV\t \n", MOISTURE_MEASUREMENT);
    }
}
