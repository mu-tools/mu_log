/**
 * MIT License
 *
 * Copyright (c) 2025 R. D. Poor & Assoc <rdpoor @ gmail.com>
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// *****************************************************************************
// Includes

#include "mu_log.h"

#if defined(MU_LOG_ENABLE) || defined(MU_LOG_ENABLE_FORMATTED) // whole file

#include <stddef.h>
#include <stdio.h>

// *****************************************************************************
// Private types and definitions

// *****************************************************************************
// Private (forward) declarations

// *****************************************************************************
// Private (static) storage

static mu_log_t s_logger = { 
    .log_fn = NULL, 
    .threshold = MU_LOG_DEFAULT_LEVEL
};

// define s_level_names[], an array that maps a logging level to a string
#define EXPAND_LEVEL_NAMES(_enum_id, _name) _name,
static const char *s_level_names[] = {MU_LOG_LEVELS(EXPAND_LEVEL_NAMES)};
#define N_LOG_LEVELS (sizeof(s_level_names)/sizeof(s_level_names[0]))

// *****************************************************************************
// Public code

void mu_log_set_fn(mu_log_fn fn) {
    s_logger.log_fn = fn;
}

mu_log_fn mu_log_get_fn(void) {
    return s_logger.log_fn;
}

void mu_log_set_threshold(mu_log_level_t threshold) {
    s_logger.threshold = threshold;
}

mu_log_level_t mu_log_get_threshold(void) {
    return s_logger.threshold;
}

#ifdef MU_LOG_ENABLE_FORMATTED
// using formatted logging
void mu_log(mu_log_level_t level, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    s_logger.log_fn(level, format, ap);
    va_end(ap);
}

#else
// using simple string logging
void mu_log(mu_log_level_t level, const char *message) {
    s_logger.log_fn(level, message);
}
#endif

bool mu_log_will_log(mu_log_level_t level) {
    return (s_logger.log_fn != NULL) && (level >= s_logger.threshold);
}

const char *mu_log_level_name(mu_log_level_t level) {
    if (level < N_LOG_LEVELS) {
        return s_level_names[level];
    } else {
        return "UNKNOWN";
    }
}

#ifdef MU_LOG_ENABLE_FORMATTED
int mu_log_stdout_fn(mu_log_level_t level, const char *format, va_list ap) {
    int n1, n2, n3;

    if (!mu_log_will_log(level)) {
        return 0;
    }

    n1 = printf("%5s: ", mu_log_level_name(level));
    if (n1 < 0) return n1;

    n2 = vprintf(format, ap);
    if (n2 < 0) return n2;

    n3 = printf("\n");
    if (n3 < 0) return n3;

    return n1 + n2 + n3;
}

#else
int mu_log_stdout_fn(mu_log_level_t level, const char *message) {
    int n;

    if (!mu_log_will_log(level)) {
        return 0;
    }
    n = fputs(mu_log_level_name(level), stdout);
    if (n < 0) return n;

    n = fputs(": ", stdout);
    if (n < 0) return n;

    n = fputs(message, stdout);
    if (n < 0) return n;

    n = fputs("\n", stdout);
    if (n < 0) return n;

    return n;
}
#endif

// *****************************************************************************
// Private (static) code


// *****************************************************************************
// End of file

#endif
