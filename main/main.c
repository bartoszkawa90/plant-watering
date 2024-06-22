#include "webui.c"

static const char *TAG_main = "MAIN";
// local variables are required because values from webui are read-only
double MAIN_MOISTURE_THRESHOLD = 25;
double MAIN_SOLAR_THRESHOLD = 3000.0;
int MAIN_MOISTURE_THRESHOLD_L = 22.0;



// MAIN APP
void app_main() {
    //   xTask for moist meter
    xTaskCreate(moisture_meter_task, "moisture meter task", 4096, NULL, 2, NULL);

    // Start task to read photodiode value
    i2c_master_init();
    xTaskCreate(&read_light_sensor_task, "read_light_sensor_task", 4096, NULL, 5, NULL);

    //   xTask for webui 
    xTaskCreate(run_webui, "Start web Interface", 4096, NULL, 5, NULL);

    while (1){
        printf("\n\n%s: Moisture: %f %%\t \n",TAG_main, MOISTURE_MEASUREMENT);
        printf("%s: Light Intensity: %d Lux\t \n", TAG_main, SOLAR_MEASUREMENT);
        vTaskDelay(pdMS_TO_TICKS(300));

        MAIN_MOISTURE_THRESHOLD = MOISTURE_THRESHOLD;
        MAIN_SOLAR_THRESHOLD = SOLAR_THRESHOLD;
        MAIN_MOISTURE_THRESHOLD_L = MAIN_MOISTURE_THRESHOLD - (MAIN_MOISTURE_THRESHOLD/10);
        // adjust moisture threshold acording to value from light sensor
        if (SOLAR_MEASUREMENT < MAIN_SOLAR_THRESHOLD) {
            MAIN_MOISTURE_THRESHOLD = 22.0;
            MAIN_MOISTURE_THRESHOLD_L = MAIN_MOISTURE_THRESHOLD - (MAIN_MOISTURE_THRESHOLD/10);
        }
        else{
            MAIN_MOISTURE_THRESHOLD = 27.0;
            MAIN_MOISTURE_THRESHOLD_L = MAIN_MOISTURE_THRESHOLD - (MAIN_MOISTURE_THRESHOLD/10);
        }
        printf("%s: Current moisture threshold : %f  and  lower threshold %d\n", TAG_main, MAIN_MOISTURE_THRESHOLD, MAIN_MOISTURE_THRESHOLD_L);
        // control water pump
        if (water_pump_state && MOISTURE_MEASUREMENT > MAIN_MOISTURE_THRESHOLD){
            // turn water off 
            printf("%s: Trying to turn pump off\n\n", TAG_main);
            gpio_set_level(PUMP_GPIO, false);
            gpio_set_level(LED_PIN, false);
            water_pump_state = !water_pump_state;
        }
        else if (!water_pump_state &&  MOISTURE_MEASUREMENT < MAIN_MOISTURE_THRESHOLD){
            // turn water on
            printf("%s: Trying to turn pump on\n\n", TAG_main);
            gpio_set_level(PUMP_GPIO, true);
            gpio_set_level(LED_PIN, true);
            water_pump_state = !water_pump_state;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
