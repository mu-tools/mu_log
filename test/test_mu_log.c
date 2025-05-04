/**
 * MIT License
 *
 * Copyright (c) 2025 R. D. Poor & Assoc <rdpoor@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 */

/**
 * @file test_mu_log.c
 * @brief Unit tests for mu_log using Unity and FFF.
 */

// *****************************************************************************
// Includes

#include "mu_log.h"
#include "unity.h"
#include "fff.h"

#include <stdio.h>

#ifdef MU_LOG_ENABLE_FORMATTED
#include <stdarg.h>
#endif

DEFINE_FFF_GLOBALS

// *****************************************************************************
// FFF Fake Functions

#ifdef MU_LOG_ENABLE_FORMATTED
FAKE_VALUE_FUNC(int, mock_print_fn, const char *, va_list);
#else
FAKE_VALUE_FUNC(int, mock_print_fn, const char *);
#endif

// Fake stdout functions for testing
FAKE_VALUE_FUNC(int, vprintf, const char *, va_list);
FAKE_VALUE_FUNC_VARARG(int, printf, const char *, ...);
FAKE_VALUE_FUNC(int, fputs, const char *, FILE *);

// *****************************************************************************
// Trampoline Function

/**
 * @brief Ensures `mu_log_will_log(level)` is respected before calling `mock_print_fn`.
 */
#ifdef MU_LOG_ENABLE_FORMATTED
int test_log_fn(mu_log_level_t level, const char *format, va_list ap) {
    if (MU_LOG_WILL_LOG(level)) {
        return mock_print_fn(format, ap);
    }
    return 0;
}
#else
int test_log_fn(mu_log_level_t level, const char *message) {
    if (MU_LOG_WILL_LOG(level)) {
        return mock_print_fn(message);
    }
    return 0;
}
#endif

// *****************************************************************************
// Setup & Teardown

void setUp(void) {
    RESET_FAKE(mock_print_fn);
    RESET_FAKE(vprintf);
    RESET_FAKE(printf);
    RESET_FAKE(fputs);
    MU_LOG_SET_FN(test_log_fn);  // Use trampoline function instead of direct mock
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_INFO);
}

void tearDown(void) {}

// *****************************************************************************
// Unit Tests

/**
 * @brief Test that mu_log_init properly initializes the global logger.
 */
void test_mu_log_init(void) {
    TEST_ASSERT_EQUAL(MU_LOG_LEVEL_INFO, MU_LOG_GET_THRESHOLD());
}

/**
 * @brief Test function setter and getter
 */
void test_mu_log_set_get_fn(void) {
    MU_LOG_SET_FN(NULL);
    TEST_ASSERT_EQUAL_PTR(NULL, MU_LOG_GET_FN());
    MU_LOG_SET_FN(mu_log_stdout_fn);
    TEST_ASSERT_EQUAL_PTR(mu_log_stdout_fn, MU_LOG_GET_FN());
}

/**
 * @brief Test threshold setter and getter
 */
void test_mu_log_set_get_threshold(void) {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_WARN);
    TEST_ASSERT_EQUAL(MU_LOG_LEVEL_WARN, MU_LOG_GET_THRESHOLD());
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_INFO);
    TEST_ASSERT_EQUAL(MU_LOG_LEVEL_INFO, MU_LOG_GET_THRESHOLD());
}

/**
 * @brief Test that mu_log properly filters messages based on level.
 */
void test_mu_log_filters_by_level(void) {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_WARN);

#ifdef MU_LOG_ENABLE_FORMATTED
    MU_LOG(MU_LOG_LEVEL_INFO, "This should not be logged: %d", 42);
    MU_LOG(MU_LOG_LEVEL_ERROR, "This should be logged: %d", 99);
#else
    MU_LOG(MU_LOG_LEVEL_INFO, "This should not be logged.");
    MU_LOG(MU_LOG_LEVEL_ERROR, "This should be logged.");
#endif

    TEST_ASSERT_EQUAL(1, mock_print_fn_fake.call_count);
}

/**
 * @brief Test that mu_log does not call the log function with a lower priority level.
 */
void test_mu_log_will_log(void) {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_WARN);

    TEST_ASSERT_FALSE(MU_LOG_WILL_LOG(MU_LOG_LEVEL_INFO));
    TEST_ASSERT_TRUE(MU_LOG_WILL_LOG(MU_LOG_LEVEL_ERROR));
}

/**
 * @brief Test that mu_log_set_threshold properly updates the global logging threshold.
 */
void test_mu_log_set_threshold(void) {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_ERROR);
    TEST_ASSERT_EQUAL(MU_LOG_LEVEL_ERROR, MU_LOG_GET_THRESHOLD());

    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_DEBUG);
    TEST_ASSERT_EQUAL(MU_LOG_LEVEL_DEBUG, MU_LOG_GET_THRESHOLD());
}

/**
 * @brief Test that mu_log properly calls the logging function.
 */
void test_mu_log_executes_logging(void) {
#ifdef MU_LOG_ENABLE_FORMATTED
    MU_LOG(MU_LOG_LEVEL_INFO, "Formatted message: %s", "test");
#else
    MU_LOG(MU_LOG_LEVEL_INFO, "Simple message.");
#endif

    TEST_ASSERT_EQUAL(1, mock_print_fn_fake.call_count);
}

void test_mu_log_level_name(void) {
    TEST_ASSERT_EQUAL_STRING("TRACE", mu_log_level_name(MU_LOG_LEVEL_TRACE));
    TEST_ASSERT_EQUAL_STRING("DEBUG", mu_log_level_name(MU_LOG_LEVEL_DEBUG));
    TEST_ASSERT_EQUAL_STRING("INFO", mu_log_level_name(MU_LOG_LEVEL_INFO));
    TEST_ASSERT_EQUAL_STRING("WARN", mu_log_level_name(MU_LOG_LEVEL_WARN));
    TEST_ASSERT_EQUAL_STRING("ERROR", mu_log_level_name(MU_LOG_LEVEL_ERROR));
    TEST_ASSERT_EQUAL_STRING("FATAL", mu_log_level_name(MU_LOG_LEVEL_FATAL));
    TEST_ASSERT_EQUAL_STRING("UNKNOWN", mu_log_level_name(-1));
}

void test_mu_log_stdout_fn_calls_stdout_fns_correctly(void) {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_INFO);
#ifdef MU_LOG_ENABLE_FORMATTED
    mu_log_stdout_fn(MU_LOG_LEVEL_INFO, "Hello, %s!", "world");

    // Ensure printf() was called twice ("INFO: " + "\n")
    TEST_ASSERT_EQUAL(2, printf_fake.call_count);
    TEST_ASSERT_EQUAL_STRING("%5s: ", printf_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_STRING("\n", printf_fake.arg0_history[1]);

    // Ensure vprintf() was called once (with format string)
    TEST_ASSERT_EQUAL(1, vprintf_fake.call_count);
    TEST_ASSERT_EQUAL_STRING("Hello, %s!", vprintf_fake.arg0_history[0]);

#else
    mu_log_stdout_fn(MU_LOG_LEVEL_INFO, "Hello, world!");

    // Ensure `fputs()` was called 4 times (level name, separator, message, newline)
    TEST_ASSERT_EQUAL(4, fputs_fake.call_count);

    // Validate expected arguments for each call
    TEST_ASSERT_EQUAL_STRING("INFO", fputs_fake.arg0_history[0]);
    TEST_ASSERT_EQUAL_STRING(": ", fputs_fake.arg0_history[1]);
    TEST_ASSERT_EQUAL_STRING("Hello, world!", fputs_fake.arg0_history[2]);
    TEST_ASSERT_EQUAL_STRING("\n", fputs_fake.arg0_history[3]);
#endif
}

void test_mu_log_stdout_fn_below_threshold(void) {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_INFO);
#ifdef MU_LOG_ENABLE_FORMATTED
    mu_log_stdout_fn(MU_LOG_LEVEL_DEBUG, "Hello, %s!", "world");

    // Ensure printf() and vprintf() were not called
    TEST_ASSERT_EQUAL(0, printf_fake.call_count);
    TEST_ASSERT_EQUAL(0, vprintf_fake.call_count);

#else
    mu_log_stdout_fn(MU_LOG_LEVEL_DEBUG, "Hello, world!");

    // Ensure `fputs()` was not called
    TEST_ASSERT_EQUAL(0, fputs_fake.call_count);
#endif
}

// *****************************************************************************
// Test Runner

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_mu_log_init);
    RUN_TEST(test_mu_log_set_get_fn);
    RUN_TEST(test_mu_log_set_get_threshold);
    RUN_TEST(test_mu_log_filters_by_level);
    RUN_TEST(test_mu_log_will_log);
    RUN_TEST(test_mu_log_set_threshold);
    RUN_TEST(test_mu_log_executes_logging);
    RUN_TEST(test_mu_log_level_name);
    RUN_TEST(test_mu_log_stdout_fn_calls_stdout_fns_correctly);
    RUN_TEST(test_mu_log_stdout_fn_below_threshold);

    return UNITY_END();
}
