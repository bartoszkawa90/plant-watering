// serwowanie samego html w formie stringa
#include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "driver/gpio.h"

#include "esp_rom_gpio.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "wifi.c"

#include <string.h>
#include "pages.c"
#include "light_sensor.c"
#include "moisture_sensor.c"

// Replace with your Wi-Fi credentials
#define WIFI_SSID "Galaxy M213EBB"
#define WIFI_PASS "wwmz5043"
#define LED_PIN 2 // GPIO pin dla niebieskiego leda z prawej
#define PUMP_GPIO 4 // GPIO for pump

//** variables
static const char *TAG_webui = "WEBUI";
static bool led_state = false;
static int nr_of_switches = 0;
static uint16_t MOISTURE_THRESHOLD = 0;
// static uint16_t SOLAR_THRESHOLD = 0;
static uint16_t default_moisture_threshold = 2000;
static uint16_t default_solar_threshold = 9000;
static bool water_pump_state = false;
static char *mv_unit = " mV ";

// additional
static uint8_t default_watering_period = 5;


esp_err_t get_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t def_moist_handler(httpd_req_t *req) {
    char str[10];
    int str_len = sprintf(str, "%d", default_moisture_threshold);
    strcat(str, mv_unit);
    httpd_resp_send(req, str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t def_solar_handler(httpd_req_t *req) {
    char str[10];
    int str_len = sprintf(str, "%d", default_solar_threshold);
    strcat(str, mv_unit);
    httpd_resp_send(req, str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t moist_val_handler(httpd_req_t *req) {
    char str[10];
    int str_len = sprintf(str, "%ld", MOISTURE_MEASUREMENT);
    strcat(str, mv_unit);
    httpd_resp_send(req, str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t solar_val_handler(httpd_req_t *req) {
    char str[10];
    int str_len = sprintf(str, "%d", SOLAR_MEASUREMENT);
    strcat(str, mv_unit);
    httpd_resp_send(req, str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t toggle_handler(httpd_req_t *req) {
    led_state = !led_state;
    nr_of_switches += 1;
    gpio_set_level(LED_PIN, led_state);
        char *resp_str[25];
    if (led_state){
        strcpy(resp_str, "ON count: ");
    }
    else{
        strcpy(resp_str, "OFF count: ");
    }
    // concatenate number of led switches to string
    char temp[20];
    snprintf(temp, sizeof(temp), "%d", nr_of_switches);
    strncat(resp_str, temp, sizeof(resp_str) - strlen(resp_str) - 1);
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// handler for turning pump on and off
esp_err_t turn_on_off_pump_handler(httpd_req_t *req) {
    water_pump_state = !water_pump_state;
    gpio_set_level(PUMP_GPIO, water_pump_state);
        char *resp_str[25];
    if (water_pump_state){
        strcpy(resp_str, "TURN OFF");
    }
    else{
        strcpy(resp_str, "TURN ON");
    }
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_toggle = {
    .uri       = "/toggle",
    .method    = HTTP_GET,
    .handler   = toggle_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_pump = {
    .uri       = "/pump",
    .method    = HTTP_GET,
    .handler   = turn_on_off_pump_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_def_moist = {
    .uri       = "/def_moist",
    .method    = HTTP_GET,
    .handler   = def_moist_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_def_solar = {
    .uri       = "/def_solar",
    .method    = HTTP_GET,
    .handler   = def_solar_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_moist_val = {
    .uri       = "/moist_val",
    .method    = HTTP_GET,
    .handler   = moist_val_handler,
    .user_ctx  = NULL
};

httpd_uri_t uri_solar_val = {
    .uri       = "/solar_val",
    .method    = HTTP_GET,
    .handler   = solar_val_handler,
    .user_ctx  = NULL
};

// start web server
httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_toggle);
        httpd_register_uri_handler(server, &uri_pump);
        httpd_register_uri_handler(server, &uri_def_moist);
        httpd_register_uri_handler(server, &uri_def_solar);
        httpd_register_uri_handler(server, &uri_moist_val);
        httpd_register_uri_handler(server, &uri_solar_val);
    }
    return server;
}


void run_webui(void *pvParameters)
{
    // connect to specific network
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    example_connect(WIFI_SSID, WIFI_PASS);

    // gpio init
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(PUMP_GPIO);
    gpio_set_direction(PUMP_GPIO, GPIO_MODE_OUTPUT);

    // Start webui
    httpd_handle_t server = start_webserver();
    if (server == NULL) {
        ESP_LOGI(TAG_webui, "Failed to start web server");
    }

    while (1) {
        // int pump_gpio_state = 0;
        // pump_gpio_state = gpio_get_level(4);
        // printf("%s: Current value of GPIO for pump : %d \n", TAG_webui, pump_gpio_state);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
