// webui
#include "webui.c"

// light sensor
// #include "light_sensor.c"

static const char *TAG_main = "MAIN";

    
// MAIN APP
void app_main() {
    //   xTask for moist meter
    xTaskCreate(moisture_meter_task, "moisture meter task", 4096, NULL, 2, NULL);

    // Start task to read photodiode value
    xTaskCreate(&read_light_sensor_task, "read_light_sensor_task", 4096, NULL, 5, NULL);

    //   xTask for webui 
    xTaskCreate(run_webui, "Start web Interface", 4096, NULL, 5, NULL);

    // while (1){
    //     vTaskDelay(pdMS_TO_TICKS(3000));
    //     printf("Moisture: %ld mV\t \n", MOISTURE_MEASUREMENT);
    // }
}
