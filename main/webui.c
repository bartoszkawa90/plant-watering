#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"

// Replace with your Wi-Fi credentials
#define WIFI_SSID "Galaxy M213EBB"
#define WIFI_PASS "wwmz5043"

// GPIO pin where the LED is connected
#define LED_GPIO_PIN 2

// Function to initialize Wi-Fi
static void wifi_init(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Wait for Wi-Fi connection
    esp_wifi_connect();
}

// Function to toggle the LED state
static esp_err_t toggle_led(httpd_req_t *req) {
    // Read the current LED state
    // int led_state = gpio_get_level(LED_GPIO_PIN);

    // Toggle the LED state
    // gpio_set_level(LED_GPIO_PIN, !led_state);
    printf("Zmiana stanu leda");

    // Respond with the new LED state
    httpd_resp_send_500(req);
    return ESP_OK;
}

// Function to initialize the HTTP server
static httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register the handler for the "/toggle" URI
        httpd_uri_t toggle_uri = {
            .uri       = "/toggle",
            .method    = HTTP_GET,
            .handler   = toggle_led,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &toggle_uri);
    }
    return server;
}

void app_main(void) {
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());
    // Initialize Wi-Fi
    wifi_init();
    // Configure the LED GPIO pin as an output
    // gpio_pad_select_gpio(LED_GPIO_PIN);
    // gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);
    // Start the web server
    start_webserver();
}
