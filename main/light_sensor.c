
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define COMMAND1_REG 0x00
#define COMMAND2_REG 0x01
#define DATA_LSB_REG 0x02
#define DATA_MSB_REG 0x03
#define INT_LT_LSB_REG 0x04
#define INT_LT_MSB_REG 0x05
#define INT_HT_LSB_REG 0x06
#define INT_HT_MSB_REG 0x07
#define TEST_REG 0x08

#define I2C_MASTER_SCL_IO    22    // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO    21    // GPIO number for I2C master data
#define I2C_MASTER_NUM       I2C_NUM_0 // I2C port number for master dev
#define ISL29023_SENSOR_ADDR 0x44  // Slave address for the light sensor
#define WRITE_BIT            I2C_MASTER_WRITE // I2C master write
#define READ_BIT             1  // I2C master read
#define ACK_CHECK_EN         0x1    // I2C master will check ack from slave
#define ACK_CHECK_DIS        0x0    // I2C master will not check ack from slave
#define ACK_VAL              0x0    // I2C ack value
#define NACK_VAL             0x1    // I2C nack value

static uint16_t SOLAR_MEASUREMENT = 0;




static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}


static esp_err_t light_sens_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, ISL29023_SENSOR_ADDR, &reg_addr, 1, data, len, 1000);
}


static esp_err_t light_sens_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, ISL29023_SENSOR_ADDR, write_buf, sizeof(write_buf), 1000);

    return ret;
}

static esp_err_t light_sens_reg_write_and_check(uint8_t reg_addr, uint8_t data){
    light_sens_register_write_byte(reg_addr, data);
    uint8_t check = 0;
    int ret = light_sens_register_read(reg_addr, &check, 1);
    printf("Check register %d, returned value: %d, expected value %d \n", reg_addr, check, data);

    return ret;
}


uint16_t read_from_ISL29023() {
    uint8_t sensor_data_h, sensor_data_l;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    // 
    light_sens_reg_write_and_check(COMMAND1_REG, 0xA0);
    light_sens_reg_write_and_check(COMMAND2_REG, 0x01);
    // read data 
    uint8_t lsb;
    uint8_t msb;
    light_sens_register_read(DATA_LSB_REG, &lsb, 1);
    light_sens_register_read(DATA_MSB_REG, &msb, 1);

    printf("Light Intensity LBS: %d \n", lsb);
    printf("Light Intensity MBS: %d \n", msb);

    // uint16_t whole_data = ((uint16_t)msb << 8 | lsb);
    // // Ecal / counting value from datasheet equation
    // uint16_t light_intensity = (whole_data * 16000/65536);
    return ((uint16_t)msb << 8 | lsb);
}


// Task to continuously read sensor data
void read_light_sensor_task(void *pvParameters) {
    i2c_master_init();
    while (1) {
        SOLAR_MEASUREMENT = read_from_ISL29023();
        printf("Light Intensity: %d \n", SOLAR_MEASUREMENT);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}