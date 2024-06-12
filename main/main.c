#include "webui.c"

static const char *TAG_main = "MAIN";

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
        vTaskDelay(pdMS_TO_TICKS(100));
        printf("%s: Moisture: %ld mV\t \n",TAG_main, MOISTURE_MEASUREMENT);
        printf("%s: Light Intensity: %d Lux\t \n", TAG_main, SOLAR_MEASUREMENT);
    }
}
