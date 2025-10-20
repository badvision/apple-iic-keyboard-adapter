/**
 * @file ay3600_emulator.h
 * @brief AY-3600-PRO-KEY Keyboard Encoder Emulator
 *
 * Software emulation of the AY-3600 keyboard encoder IC used in the Apple IIc.
 * Provides the same output signals and behavior as the original hardware.
 *
 * Original AY-3600 Output Signals:
 * - D0-D4: 5-bit key code (0-31)
 * - CONTROL: Control key state
 * - SHIFT: Shift key state
 * - ANY-KEY: High when any key is pressed
 * - KSTRB: Keyboard strobe (pulses on key press)
 *
 * @note This emulator handles debouncing, key repeat, and timing to match
 *       the original AY-3600 behavior as documented in the Apple IIc
 *       Technical Reference Manual, Section 11.7.
 */

#ifndef AY3600_EMULATOR_H
#define AY3600_EMULATOR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum key code value (5-bit = 0-31)
 */
#define AY3600_MAX_KEY_CODE 31

/**
 * @brief AY-3600 output signal structure
 *
 * Represents the state of all output signals from the AY-3600 encoder.
 */
typedef struct {
    uint8_t key_code;    /**< 5-bit key code (0-31) on D0-D4 lines */
    bool control;        /**< CONTROL key state */
    bool shift;          /**< SHIFT key state */
    bool any_key;        /**< ANY-KEY signal (high when key pressed) */
    bool strobe;         /**< KSTRB signal (pulse on key press) */
} ay3600_output_t;

/**
 * @brief Callback function type for output signal changes
 *
 * Called whenever the AY-3600 output signals change.
 *
 * @param output Pointer to current output state
 */
typedef void (*ay3600_output_callback_t)(const ay3600_output_t *output);

/**
 * @brief AY-3600 emulator configuration
 */
typedef struct {
    ay3600_output_callback_t output_callback;  /**< Callback for output changes */
    uint16_t debounce_ms;                      /**< Debounce time in milliseconds */
    uint16_t repeat_delay_ms;                  /**< Initial repeat delay (default 500ms) */
    uint16_t repeat_rate_ms;                   /**< Repeat rate (default 50ms = 20 Hz) */
} ay3600_config_t;

/**
 * @brief Key event structure
 *
 * Represents a single key event from input source (USB/BT/matrix).
 */
typedef struct {
    uint8_t key_code;    /**< Apple IIc key code (0-31) */
    bool control;        /**< Control modifier state */
    bool shift;          /**< Shift modifier state */
    bool pressed;        /**< True if key pressed, false if released */
} ay3600_key_event_t;

/**
 * @brief Initialize the AY-3600 emulator
 *
 * Must be called before any other AY-3600 functions.
 *
 * @param config Configuration structure
 * @return 0 on success, negative error code on failure
 */
int ay3600_init(const ay3600_config_t *config);

/**
 * @brief Process pending key events
 *
 * Should be called periodically (e.g., every 1ms) to handle debouncing
 * and key repeat timing.
 */
void ay3600_process(void);

/**
 * @brief Press a key
 *
 * Simulates pressing a key on the keyboard matrix.
 *
 * @param key_code Apple IIc key code (0-31)
 * @param control Control modifier state
 * @param shift Shift modifier state
 * @return 0 on success, negative error code on failure
 */
int ay3600_press_key(uint8_t key_code, bool control, bool shift);

/**
 * @brief Release the currently pressed key
 *
 * @return 0 on success, negative error code on failure
 */
int ay3600_release_key(void);

/**
 * @brief Handle a key event
 *
 * Processes a complete key event (press or release).
 *
 * @param event Key event structure
 * @return 0 on success, negative error code on failure
 */
int ay3600_handle_event(const ay3600_key_event_t *event);

/**
 * @brief Get current output state
 *
 * @param output Pointer to structure to fill with current state
 * @return 0 on success, negative error code on failure
 */
int ay3600_get_output(ay3600_output_t *output);

/**
 * @brief Reset the emulator to idle state
 *
 * Clears all pending events and resets output signals.
 */
void ay3600_reset(void);

/**
 * @brief Get emulator statistics
 *
 * Useful for debugging and performance monitoring.
 */
typedef struct {
    uint32_t total_keypresses;    /**< Total number of key presses */
    uint32_t total_repeats;       /**< Total number of repeated keys */
    uint32_t debounce_events;     /**< Number of debounced events */
} ay3600_stats_t;

/**
 * @brief Get emulator statistics
 *
 * @param stats Pointer to structure to fill with statistics
 */
void ay3600_get_stats(ay3600_stats_t *stats);

#ifdef __cplusplus
}
#endif

#endif /* AY3600_EMULATOR_H */
