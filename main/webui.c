// serwowanie samego html w formie stringa
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

// Replace with your Wi-Fi credentials
#define WIFI_SSID "Galaxy M213EBB"
#define WIFI_PASS "wwmz5043"
#define LED_PIN 2 // GPIO pin dla niebieskiego leda z prawej

//** variables
static const char *TAG_webui = "WEBUI";
static bool led_state = false;
static int nr_of_switches = 0;
static uint16_t MOISTURE_MEASUREMENT = 0;
static uint16_t SOLAR_VALUE = 0;
static uint16_t MOISTURE_THRESHOLD = 0;
static uint16_t SOLAR_THRESHOLD = 0;
static uint8_t default_watering_period = 5;

//**

/* An HTTP GET handler for the root page */
esp_err_t get_handler(httpd_req_t *req) {
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* An HTTP GET handler to toggle the LED */
esp_err_t toggle_handler(httpd_req_t *req) {
    // static bool led_state = false;
    led_state = !led_state;
    nr_of_switches += 1;
    gpio_set_level(LED_PIN, led_state);
        char *resp_str[25];
    if (led_state){
        strcpy(resp_str, "ON count: ");
        // resp_str = "ON count: ";
    }
    else{
        strcpy(resp_str, "OFF count: ");
        // resp_str = "OFF count: ";
    }
    // concatenate number of led switches to string
    char temp[20];
    snprintf(temp, sizeof(temp), "%d", nr_of_switches);
    strncat(resp_str, temp, sizeof(resp_str) - strlen(resp_str) - 1);
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = NULL
};

// handler to /toggle uri
httpd_uri_t uri_toggle = {
    .uri       = "/toggle",
    .method    = HTTP_GET,
    .handler   = toggle_handler,
    .user_ctx  = NULL
};

// start web server
httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_toggle);
    }
    return server;
}

// ADDITIONAL FUNCTIONS
char* load_file(char * filepath){
    // load file to buffer
    ESP_LOGI(TAG_webui, "Start to read file");
    FILE *file = fopen(filepath, "r");
    if (file == NULL){
        ESP_LOGE(TAG_webui, "Cant open file");
    }
    int MAX_LEN = 5000;
    char f_buffer[MAX_LEN];
    while (fgets(f_buffer, MAX_LEN, file)){
        printf("%s", f_buffer);
    }

    return f_buffer;
}

// void run_webui(void *pvParameters)
void run_webui()
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

    // Initialize the GPIO pin for the LED
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Start the webserver
    httpd_handle_t server = start_webserver();
    if (server == NULL) {
        ESP_LOGI(TAG_webui, "Failed to start web server");
    }
}