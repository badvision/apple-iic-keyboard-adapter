/**
 * @file ay3600_emulator.c
 * @brief AY-3600-PRO-KEY Keyboard Encoder Emulator Implementation
 */

#include "ay3600_emulator.h"
#include <string.h>

#ifndef NATIVE_TEST
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#define LOG_TAG "ay3600"
#define LOG_DEBUG(fmt, ...) ESP_LOGD(LOG_TAG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  ESP_LOGI(LOG_TAG, fmt, ##__VA_ARGS__)
#define GET_TIME_MS()       (xTaskGetTickCount() * portTICK_PERIOD_MS)
#else
#include <stdio.h>
#include <time.h>
#define LOG_DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  printf("[INFO] " fmt "\n", ##__VA_ARGS__)
static uint32_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}
#define GET_TIME_MS() get_time_ms()
#endif

/**
 * @brief Emulator state machine states
 */
typedef enum {
    STATE_IDLE,           /**< No key pressed */
    STATE_DEBOUNCE,       /**< Debouncing key press */
    STATE_PRESSED,        /**< Key is pressed and stable */
    STATE_REPEAT_DELAY,   /**< Waiting for initial repeat delay */
    STATE_REPEATING,      /**< Key is auto-repeating */
} ay3600_state_t;

/**
 * @brief Internal emulator state
 */
typedef struct {
    ay3600_config_t config;          /**< Configuration */
    ay3600_output_t output;          /**< Current output state */
    ay3600_state_t state;            /**< State machine state */
    ay3600_stats_t stats;            /**< Statistics */

    uint8_t current_key;             /**< Currently pressed key code */
    bool current_control;            /**< Current control state */
    bool current_shift;              /**< Current shift state */

    uint32_t last_change_time;       /**< Time of last state change */
    uint32_t last_repeat_time;       /**< Time of last repeat */
} ay3600_state_internal_t;

static ay3600_state_internal_t g_state;

/**
 * @brief Update output signals and call callback
 */
static void update_output(void)
{
    if (g_state.config.output_callback) {
        g_state.config.output_callback(&g_state.output);
    }
}

/**
 * @brief Set output signals for a key press
 */
static void set_key_output(uint8_t key_code, bool control, bool shift)
{
    g_state.output.key_code = key_code & 0x1F; // Mask to 5 bits
    g_state.output.control = control;
    g_state.output.shift = shift;
    g_state.output.any_key = true;
    g_state.output.strobe = true; // Will pulse in callback

    LOG_DEBUG("Key output: code=0x%02X, ctrl=%d, shift=%d",
              key_code, control, shift);

    update_output();

    // Clear strobe after pulse
    g_state.output.strobe = false;
}

/**
 * @brief Clear output signals (key released)
 */
static void clear_output(void)
{
    g_state.output.key_code = 0;
    g_state.output.control = false;
    g_state.output.shift = false;
    g_state.output.any_key = false;
    g_state.output.strobe = false;

    LOG_DEBUG("Output cleared");

    update_output();
}

int ay3600_init(const ay3600_config_t *config)
{
    if (!config) {
        return -1;
    }

    memset(&g_state, 0, sizeof(g_state));
    g_state.config = *config;
    g_state.state = STATE_IDLE;

    LOG_INFO("AY3600 emulator initialized (debounce=%dms, repeat_delay=%dms, repeat_rate=%dms)",
             config->debounce_ms, config->repeat_delay_ms, config->repeat_rate_ms);

    return 0;
}

void ay3600_process(void)
{
    uint32_t now = GET_TIME_MS();
    uint32_t elapsed;

    switch (g_state.state) {
        case STATE_IDLE:
            // Nothing to do
            break;

        case STATE_DEBOUNCE:
            elapsed = now - g_state.last_change_time;
            if (elapsed >= g_state.config.debounce_ms) {
                // Debounce complete, move to pressed state
                g_state.state = STATE_PRESSED;
                g_state.last_change_time = now;
                g_state.last_repeat_time = now;

                // Output the key
                set_key_output(g_state.current_key,
                             g_state.current_control,
                             g_state.current_shift);

                g_state.stats.total_keypresses++;
            }
            break;

        case STATE_PRESSED:
            elapsed = now - g_state.last_change_time;
            if (elapsed >= g_state.config.repeat_delay_ms) {
                // Initial repeat delay elapsed, start repeating
                g_state.state = STATE_REPEATING;
                g_state.last_repeat_time = now;

                // Output repeat
                set_key_output(g_state.current_key,
                             g_state.current_control,
                             g_state.current_shift);

                g_state.stats.total_repeats++;
            }
            break;

        case STATE_REPEATING:
            elapsed = now - g_state.last_repeat_time;
            if (elapsed >= g_state.config.repeat_rate_ms) {
                // Repeat rate interval elapsed, output key again
                g_state.last_repeat_time = now;

                set_key_output(g_state.current_key,
                             g_state.current_control,
                             g_state.current_shift);

                g_state.stats.total_repeats++;
            }
            break;
    }
}

int ay3600_press_key(uint8_t key_code, bool control, bool shift)
{
    if (key_code > AY3600_MAX_KEY_CODE) {
        return -1;
    }

    LOG_DEBUG("Key pressed: code=0x%02X, ctrl=%d, shift=%d", key_code, control, shift);

    g_state.current_key = key_code;
    g_state.current_control = control;
    g_state.current_shift = shift;

    if (g_state.config.debounce_ms > 0) {
        // Start debounce timer
        g_state.state = STATE_DEBOUNCE;
        g_state.last_change_time = GET_TIME_MS();
        g_state.stats.debounce_events++;
    } else {
        // No debounce, go directly to pressed
        g_state.state = STATE_PRESSED;
        g_state.last_change_time = GET_TIME_MS();
        g_state.last_repeat_time = GET_TIME_MS();

        set_key_output(key_code, control, shift);
        g_state.stats.total_keypresses++;
    }

    return 0;
}

int ay3600_release_key(void)
{
    LOG_DEBUG("Key released");

    g_state.state = STATE_IDLE;
    g_state.current_key = 0;
    g_state.current_control = false;
    g_state.current_shift = false;

    clear_output();

    return 0;
}

int ay3600_handle_event(const ay3600_key_event_t *event)
{
    if (!event) {
        return -1;
    }

    if (event->pressed) {
        return ay3600_press_key(event->key_code, event->control, event->shift);
    } else {
        return ay3600_release_key();
    }
}

int ay3600_get_output(ay3600_output_t *output)
{
    if (!output) {
        return -1;
    }

    *output = g_state.output;
    return 0;
}

void ay3600_reset(void)
{
    LOG_INFO("Resetting emulator");

    g_state.state = STATE_IDLE;
    g_state.current_key = 0;
    g_state.current_control = false;
    g_state.current_shift = false;
    clear_output();
}

void ay3600_get_stats(ay3600_stats_t *stats)
{
    if (stats) {
        *stats = g_state.stats;
    }
}
