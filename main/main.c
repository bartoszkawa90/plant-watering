// standard includes for main
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "driver/i2c.h"


// webui.c
#include "webui.c"

static const char *TAG_main = "MAIN";

// ACD / moisture measurements
#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc2_channel_t channel = ADC_CHANNEL_0;     
static const adc_atten_t atten = ADC_ATTEN_DB_12;
static const adc_unit_t unit = ADC_UNIT_1;
// static uint32_t MOISTURE_MEASUREMENT = 0;


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
        printf("Raw: %ld\tVoltage: %ldmV\n", adc_reading, voltage);
        MOISTURE_MEASUREMENT = voltage;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



//------------------------------------------------------------------------------------------------------------------------

// I2C Master Configuration
#define I2C_MASTER_SCL_IO          22  // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO          21  // GPIO number for I2C master data
#define I2C_MASTER_NUM             I2C_NUM_0  // I2C port number for master
#define I2C_MASTER_FREQ_HZ         100000     // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE  0          // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE  0          // I2C master doesn't need buffer
#define I2C_MASTER_TIMEOUT_MS      1000

// BOOSTXL-SENSHUB light sensor I2C Address
#define PHOTODIODE_I2C_ADDRESS     0x44  


// i2c init
esp_err_t i2c_master_init() {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode,
                              I2C_MASTER_TX_BUF_DISABLE, I2C_MASTER_RX_BUF_DISABLE, 0);
}

void read_photodiode_task(void *pvParameters) {

    while (1){
        uint8_t sensor_data[2];
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (PHOTODIODE_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);

        // ??? niby działa tak samo
        uint8_t data[2];
        i2c_master_read_from_device(I2C_MASTER_NUM, PHOTODIODE_I2C_ADDRESS, data, 2, I2C_MASTER_TIMEOUT_MS);
        ESP_LOGI("PHOTODIODE", "Photodiode Value: %d", data[0]);
        ESP_LOGI("PHOTODIODE", "Photodiode Value: %d", data[1]);
        // ???

        i2c_master_read(cmd, sensor_data, 2, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
        i2c_cmd_link_delete(cmd);

        if (ret == ESP_OK) {
            ESP_LOGI("PHOTODIODE", "Photodiode Value: %d", sensor_data[0]);
            ESP_LOGI("PHOTODIODE", "Photodiode Value: %d", sensor_data[1]);
            int photodiode_value = (sensor_data[0] << 8) | sensor_data[1];
            ESP_LOGI("PHOTODIODE", "Photodiode Value: %d", photodiode_value);
        } else {
            ESP_LOGE("PHOTODIODE", "I2C read failed");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}




//------------------------------------------------------------------------------------------------------------------------



    
// MAIN APP
void app_main() {

    // run_webui();


    //   xTask for moist meter
    xTaskCreate(moisture_meter_task, "moisture meter task", 4096, NULL, 2, NULL);

    //   xTask for solar meter
    // Initialize I2C master
    ESP_ERROR_CHECK(i2c_master_init());
    // Start task to read photodiode value
    xTaskCreate(read_photodiode_task, "read_photodiode_task", 4096, NULL, 2, NULL);

    //   xTask for webui 
    xTaskCreate(run_webui, "Start web Interface", 4096, NULL, 5, NULL);

    // vTaskStartScheduler();

    while (1){
        vTaskDelay(pdMS_TO_TICKS(3000));
        printf("Moisture: %ld mV\t \n", MOISTURE_MEASUREMENT);
    }
}

