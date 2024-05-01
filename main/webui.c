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

// Function to serve a simple HTML page
static esp_err_t serve_html(httpd_req_t *req) {
    const char* resp_str = "<!DOCTYPE html>"
                           "<html>"
                           "<head><title>ESP32 Web Server</title></head>"
                           "<body>"
                           "<h1>Hello from ESP32!</h1>"
                           "<p>LED is now: %s</p>"
                           "<a href=\"/toggle\">Toggle LED</a>"
                           "</body>"
                           "</html>";
    // Read the current LED state
    // int led_state = gpio_get_level(LED_GPIO_PIN);
    char* buf;
    asprintf(&buf, resp_str ? "ON" : "OFF");

    // Send the HTML content
    httpd_resp_send(req, buf, strlen(buf));
    free(buf);
    return ESP_OK;
}

// Modify the start_webserver function to use serve_html
static void start_webserver(void *pvParameters) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register the handler for the root URI to serve the HTML page
        httpd_uri_t root_uri = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = serve_html,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &root_uri);

        // Keep the toggle URI handler
        httpd_uri_t toggle_uri = {
            .uri       = "/toggle",
            .method    = HTTP_GET,
            .handler   = toggle_led, // Keep the existing toggle handler
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &toggle_uri);
    }

    // Delete the task if the server fails to start
    if (server == NULL) {
        vTaskDelete(NULL);
    }

    // The server is now running; keep the task alive
    while (1) {
        vTaskDelay(portMAX_DELAY);
    }
}

// void app_main(void) {
//     // Initialize NVS
//     ESP_ERROR_CHECK(nvs_flash_init());
//     // Initialize Wi-Fi
//     wifi_init();
//     // Configure the LED GPIO pin as an output
//     gpio_pad_select_gpio(LED_GPIO_PIN);
//     gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

//     // Create a task to start the web server
//     xTaskCreate(start_webserver, "start_webserver", 4096, NULL, 5, NULL);
// }


