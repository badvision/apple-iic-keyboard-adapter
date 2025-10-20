/**
 * @file main.c
 * @brief Apple IIc USB/Bluetooth Keyboard Adapter
 *
 * Main application entry point for ESP32-C3 based keyboard adapter
 * that replaces the AY-3600-PRO-KEY encoder in the Apple IIc.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "ay3600_emulator.h"

static const char *TAG = "main";

// GPIO pin definitions (match your hardware)
#define PIN_D0      GPIO_NUM_0
#define PIN_D1      GPIO_NUM_1
#define PIN_D2      GPIO_NUM_2
#define PIN_D3      GPIO_NUM_3
#define PIN_D4      GPIO_NUM_4
#define PIN_CONTROL GPIO_NUM_5
#define PIN_SHIFT   GPIO_NUM_6
#define PIN_ANY_KEY GPIO_NUM_7
#define PIN_KSTRB   GPIO_NUM_8

/**
 * @brief Initialize GPIO pins for AY3600 output signals
 */
static void init_gpio(void)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << PIN_D0) | (1ULL << PIN_D1) |
                        (1ULL << PIN_D2) | (1ULL << PIN_D3) |
                        (1ULL << PIN_D4) | (1ULL << PIN_CONTROL) |
                        (1ULL << PIN_SHIFT) | (1ULL << PIN_ANY_KEY) |
                        (1ULL << PIN_KSTRB),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_conf);

    // Initialize all outputs to LOW
    gpio_set_level(PIN_D0, 0);
    gpio_set_level(PIN_D1, 0);
    gpio_set_level(PIN_D2, 0);
    gpio_set_level(PIN_D3, 0);
    gpio_set_level(PIN_D4, 0);
    gpio_set_level(PIN_CONTROL, 0);
    gpio_set_level(PIN_SHIFT, 0);
    gpio_set_level(PIN_ANY_KEY, 0);
    gpio_set_level(PIN_KSTRB, 0);
}

/**
 * @brief GPIO output callback for AY3600 emulator
 */
static void gpio_output_callback(const ay3600_output_t *output)
{
    // Set data lines (D0-D4)
    gpio_set_level(PIN_D0, (output->key_code >> 0) & 1);
    gpio_set_level(PIN_D1, (output->key_code >> 1) & 1);
    gpio_set_level(PIN_D2, (output->key_code >> 2) & 1);
    gpio_set_level(PIN_D3, (output->key_code >> 3) & 1);
    gpio_set_level(PIN_D4, (output->key_code >> 4) & 1);

    // Set modifier lines
    gpio_set_level(PIN_CONTROL, output->control);
    gpio_set_level(PIN_SHIFT, output->shift);
    gpio_set_level(PIN_ANY_KEY, output->any_key);

    // Pulse KSTRB if key is pressed
    if (output->any_key) {
        gpio_set_level(PIN_KSTRB, 1);
        esp_rom_delay_us(1); // 1μs pulse
        gpio_set_level(PIN_KSTRB, 0);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Apple IIc Keyboard Adapter starting...");

    // Initialize GPIO
    init_gpio();
    ESP_LOGI(TAG, "GPIO initialized");

    // Initialize AY3600 emulator
    ay3600_config_t config = {
        .output_callback = gpio_output_callback,
        .debounce_ms = 20,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };

    ay3600_init(&config);
    ESP_LOGI(TAG, "AY3600 emulator initialized");

    // TODO: Initialize USB Host
    // TODO: Initialize Bluetooth

    // Test: Send a test keystroke
    ESP_LOGI(TAG, "Sending test 'A' keystroke...");
    ay3600_press_key(0x00, false, false); // 'A' key
    vTaskDelay(pdMS_TO_TICKS(100));
    ay3600_release_key();

    ESP_LOGI(TAG, "Initialization complete. Entering main loop...");

    // Main loop
    while (1) {
        // Process keyboard events
        ay3600_process();

        vTaskDelay(pdMS_TO_TICKS(1)); // 1ms tick
    }
}
