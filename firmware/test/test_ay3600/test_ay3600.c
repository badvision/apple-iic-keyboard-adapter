/**
 * @file test_ay3600.c
 * @brief Unit tests for AY-3600 emulator
 */

#include "unity.h"
#include "ay3600_emulator.h"
#include <string.h>

// Test fixture data
static ay3600_output_t last_output;
static int callback_count;

// Test callback
static void test_callback(const ay3600_output_t *output)
{
    last_output = *output;
    callback_count++;
}

void setUp(void)
{
    memset(&last_output, 0, sizeof(last_output));
    callback_count = 0;
}

void tearDown(void)
{
    ay3600_reset();
}

// Test initialization
void test_ay3600_init_success(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 20,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };

    int result = ay3600_init(&config);
    TEST_ASSERT_EQUAL(0, result);
}

void test_ay3600_init_null_config(void)
{
    int result = ay3600_init(NULL);
    TEST_ASSERT_EQUAL(-1, result);
}

// Test basic key press
void test_ay3600_press_key_valid(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0, // No debounce for faster test
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press 'A' key (code 0x00)
    int result = ay3600_press_key(0x00, false, false);
    TEST_ASSERT_EQUAL(0, result);

    // Verify output
    TEST_ASSERT_EQUAL(0x00, last_output.key_code);
    TEST_ASSERT_FALSE(last_output.control);
    TEST_ASSERT_FALSE(last_output.shift);
    TEST_ASSERT_TRUE(last_output.any_key);
}

void test_ay3600_press_key_invalid_code(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Try to press invalid key (> 31)
    int result = ay3600_press_key(32, false, false);
    TEST_ASSERT_EQUAL(-1, result);
}

void test_ay3600_press_key_with_modifiers(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press 'A' with Control
    ay3600_press_key(0x00, true, false);
    TEST_ASSERT_EQUAL(0x00, last_output.key_code);
    TEST_ASSERT_TRUE(last_output.control);
    TEST_ASSERT_FALSE(last_output.shift);
    TEST_ASSERT_TRUE(last_output.any_key);

    // Press 'A' with Shift
    ay3600_release_key();
    callback_count = 0;
    ay3600_press_key(0x00, false, true);
    TEST_ASSERT_EQUAL(0x00, last_output.key_code);
    TEST_ASSERT_FALSE(last_output.control);
    TEST_ASSERT_TRUE(last_output.shift);
    TEST_ASSERT_TRUE(last_output.any_key);

    // Press 'A' with both
    ay3600_release_key();
    callback_count = 0;
    ay3600_press_key(0x00, true, true);
    TEST_ASSERT_EQUAL(0x00, last_output.key_code);
    TEST_ASSERT_TRUE(last_output.control);
    TEST_ASSERT_TRUE(last_output.shift);
    TEST_ASSERT_TRUE(last_output.any_key);
}

// Test key release
void test_ay3600_release_key(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press then release
    ay3600_press_key(0x00, false, false);
    TEST_ASSERT_TRUE(last_output.any_key);

    int result = ay3600_release_key();
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(last_output.any_key);
    TEST_ASSERT_EQUAL(0, last_output.key_code);
}

// Test key event handling
void test_ay3600_handle_event_press(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    ay3600_key_event_t event = {
        .key_code = 0x05,
        .control = true,
        .shift = false,
        .pressed = true,
    };

    int result = ay3600_handle_event(&event);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0x05, last_output.key_code);
    TEST_ASSERT_TRUE(last_output.control);
    TEST_ASSERT_TRUE(last_output.any_key);
}

void test_ay3600_handle_event_release(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press first
    ay3600_key_event_t press_event = {
        .key_code = 0x05,
        .control = false,
        .shift = false,
        .pressed = true,
    };
    ay3600_handle_event(&press_event);

    // Then release
    ay3600_key_event_t release_event = {
        .pressed = false,
    };
    int result = ay3600_handle_event(&release_event);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_FALSE(last_output.any_key);
}

void test_ay3600_handle_event_null(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    int result = ay3600_handle_event(NULL);
    TEST_ASSERT_EQUAL(-1, result);
}

// Test get output
void test_ay3600_get_output(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    ay3600_press_key(0x0A, true, false);

    ay3600_output_t output;
    int result = ay3600_get_output(&output);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0x0A, output.key_code);
    TEST_ASSERT_TRUE(output.control);
    TEST_ASSERT_TRUE(output.any_key);
}

void test_ay3600_get_output_null(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    int result = ay3600_get_output(NULL);
    TEST_ASSERT_EQUAL(-1, result);
}

// Test reset
void test_ay3600_reset(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press a key
    ay3600_press_key(0x0F, true, true);
    TEST_ASSERT_TRUE(last_output.any_key);

    // Reset
    ay3600_reset();
    TEST_ASSERT_FALSE(last_output.any_key);
    TEST_ASSERT_EQUAL(0, last_output.key_code);
}

// Test statistics
void test_ay3600_stats(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press and release multiple times
    for (int i = 0; i < 5; i++) {
        ay3600_press_key(0x00, false, false);
        ay3600_release_key();
    }

    ay3600_stats_t stats;
    ay3600_get_stats(&stats);

    TEST_ASSERT_EQUAL(5, stats.total_keypresses);
}

// Test 5-bit masking
void test_ay3600_key_code_masking(void)
{
    ay3600_config_t config = {
        .output_callback = test_callback,
        .debounce_ms = 0,
        .repeat_delay_ms = 500,
        .repeat_rate_ms = 50,
    };
    ay3600_init(&config);

    // Press key with code that would overflow 5 bits
    ay3600_press_key(0x1F, false, false); // Max valid (31)
    TEST_ASSERT_EQUAL(0x1F, last_output.key_code);
}

// Main test runner
int main(void)
{
    UNITY_BEGIN();

    // Initialization tests
    RUN_TEST(test_ay3600_init_success);
    RUN_TEST(test_ay3600_init_null_config);

    // Basic key press tests
    RUN_TEST(test_ay3600_press_key_valid);
    RUN_TEST(test_ay3600_press_key_invalid_code);
    RUN_TEST(test_ay3600_press_key_with_modifiers);

    // Key release tests
    RUN_TEST(test_ay3600_release_key);

    // Event handling tests
    RUN_TEST(test_ay3600_handle_event_press);
    RUN_TEST(test_ay3600_handle_event_release);
    RUN_TEST(test_ay3600_handle_event_null);

    // Get output tests
    RUN_TEST(test_ay3600_get_output);
    RUN_TEST(test_ay3600_get_output_null);

    // Reset test
    RUN_TEST(test_ay3600_reset);

    // Statistics test
    RUN_TEST(test_ay3600_stats);

    // Edge case tests
    RUN_TEST(test_ay3600_key_code_masking);

    return UNITY_END();
}
