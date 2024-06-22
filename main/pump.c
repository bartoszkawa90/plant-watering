#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>


// static uint16_t MOISTURE_THRESHOLD = 2000;
// static uint16_t SOLAR_THRESHOLD = 2000;
// static bool water_pump_state = false;
// SOLAR_MEASUREMENT
// MOISTURE_MEASUREMENT
static uint16_t MOISTURE_THRESHOLD_L = MOISTURE_THRESHOLD - (MOISTURE_THRESHOLD/10);
static uint16_t SOLAR_THRESHOLD_L = SOLAR_THRESHOLD - (SOLAR_THRESHOLD/10);


void water_pump_task(void *pvParameters)
{
    while (1) {
        if (water_pump_state && MOISTURE_MEASUREMENT > MOISTURE_THRESHOLD_L && SOLAR_MEASUREMENT < SOLAR_THRESHOLD_L){
            // turn off water pump
        }
        else if (!water_pump_state && MOISTURE_MEASUREMENT < MOISTURE_THRESHOLD_L && SOLAR_MEASUREMENT > SOLAR_THRESHOLD_L){
            // turn on water pump
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}