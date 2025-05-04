# mu_log - Lightweight Logging for Embedded Systems

`mu_log` is a simple, header-only (mostly) logging module designed for resource-constrained embedded systems. It provides configurable logging severity levels, a pluggable output mechanism, and compile-time disabling of logging features to minimize code size when not needed.

## Features

* **Configurable Log Levels:** Standard severity levels (TRACE, DEBUG, INFO, WARN, ERROR, FATAL).
* **Logging Threshold:** Set a minimum severity level to filter out less important messages.
* **Pluggable Output:** Define your own logging function (`mu_log_fn`) to direct output to any destination (e.g., UART, console, memory buffer, network).
* **Optional Formatting:** Compile with `MU_LOG_ENABLE_FORMATTED` to enable `printf`-style formatting using `va_list`. Otherwise, use simple string messages.
* **Compile-Time Disabling:** Disable logging entirely by omitting `MU_LOG_ENABLE` and `MU_LOG_ENABLE_FORMATTED` defines, resulting in no-op macros and minimal code footprint.
* **Default Output:** Provides a basic `mu_log_stdout_fn` for simple console output (requires standard I/O functions like `printf` or `fputs`).

## Configuration

Logging behavior is controlled by preprocessor defines:

* `MU_LOG_ENABLE`: Enables basic logging functionality. Messages are logged as simple strings (`const char*`).
* `MU_LOG_ENABLE_FORMATTED`: Enables logging with `printf`-style formatting. This also enables basic logging. **If `MU_LOG_ENABLE_FORMATTED` is defined, `MU_LOG_ENABLE` is implicitly handled.**

Define one of these symbols in your build system or before including `mu_log.h`.

```c
// In your build system (e.g., CFLAGS): -DMU_LOG_ENABLE_FORMATTED
// OR in a common header or directly before include:
#define MU_LOG_ENABLE_FORMATTED
#include "mu_log.h"
```

If neither define is present, all MU_LOG_ macros become no-operations, and logging code is compiled out.
API

The module provides functions and convenience macros for interacting with the logger. The macros are generally preferred for ease of use and compile-time control.

    mu_log_level_t: Enumeration for log levels (TRACE, DEBUG, INFO, WARN, ERROR, FATAL). MU_LOG_DEFAULT_LEVEL is INFO.
    mu_log_fn: Function pointer type for your custom logging output function. Signature depends on MU_LOG_ENABLE_FORMATTED.
    MU_LOG_SET_FN(fn): Set the user-defined logging function.
    MU_LOG_SET_THRESHOLD(level): Set the minimum severity level to log.
    MU_LOG_GET_THRESHOLD(): Get the current logging threshold.
    MU_LOG(level, ...): Log a message with a specific level. Use level-specific macros instead.
    MU_LOG_TRACE(...), MU_LOG_DEBUG(...), MU_LOG_INFO(...), MU_LOG_WARN(...), MU_LOG_ERROR(...), MU_LOG_FATAL(...): Convenience macros for logging at specific levels. Use these in your application code.
    MU_LOG_WILL_LOG(level): Check if a message at the given level would currently be logged (useful for conditionally preparing expensive log messages).
    mu_log_level_name(level): Get the string name for a log level (e.g., "INFO").
    mu_log_stdout_fn: A provided logging function that outputs to standard output (requires <stdio.h>).

## Sample Usage

Here are examples demonstrating how to use mu_log.

### Example 1: Basic Logging with stdout

This example shows setting up logging to use the default stdout function and logging messages at different levels.

```c
#define MU_LOG_ENABLE_FORMATTED // Enable formatted logging
#include "mu_log.h"
#include <stdio.h> // Required for mu_log_stdout_fn and printf in sample

// Assume mu_log_stdout_fn is implemented elsewhere, maybe in mu_log.c

// In your application's initialization code (e.g., main function):
void app_init() {
    // Set the logging output function to the default stdout function
    // Note: mu_log_stdout_fn is assumed to be implemented in mu_log.c
    MU_LOG_SET_FN(mu_log_stdout_fn);

    // Set the minimum level of messages to log (e.g., only log INFO, WARN, ERROR, FATAL)
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_INFO);

    MU_LOG_TRACE("This trace message will likely be filtered out."); // Below INFO threshold
    MU_LOG_DEBUG("Debugging initialization step 1.");           // Below INFO threshold
    MU_LOG_INFO("Application initialization complete.");        // INFO >= INFO - Logs
    MU_LOG_WARN("Configuration file not found, using defaults."); // WARN >= INFO - Logs
}

// In other parts of your application code:
void process_data(int value) {
    MU_LOG_DEBUG("Processing value: %d", value); // Logs only if threshold <= DEBUG
    if (value < 0) {
        MU_LOG_ERROR("Invalid value received: %d", value); // Logs if threshold <= ERROR
    }
}

// Example demonstrating threshold change and MU_LOG_WILL_LOG
void demonstrate_threshold() {
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_DEBUG); // Lower threshold temporarily
    MU_LOG_DEBUG("This debug message should now appear.");

    if (MU_LOG_WILL_LOG(MU_LOG_LEVEL_DEBUG)) {
        // Prepare a potentially expensive debug string only if it will be logged
        // char debug_str[64];
        // snprintf(debug_str, sizeof(debug_str), "Complex state: %d,%s", state_val, status_str);
        // MU_LOG_DEBUG("Expensive info: %s", debug_str);
        MU_LOG_DEBUG("Logging is enabled for DEBUG level.");
    }

    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_ERROR); // Raise threshold back
    MU_LOG_INFO("This info message should now be suppressed."); // Below new threshold
}

// In a critical error path:
void handle_fatal_error() {
     MU_LOG_FATAL("Unrecoverable error occurred!"); // FATAL messages always log (highest level)
     // Perform system shutdown or reset...
}
```

### Example 2: Custom Logging Function

If you cannot use stdout (e.g., no console) or need to send logs elsewhere (e.g. 
UART, network, memory buffer), you can implement your own mu_log_fn as follows:

```c
#define MU_LOG_ENABLE // Or MU_LOG_ENABLE_FORMATTED
#include "mu_log.h"
#include <string.h> // For strcmp if needed
#include <stdarg.h> // Required for va_list if using formatted logging

// Your custom log output function
// Signature depends on MU_LOG_ENABLE_FORMATTED
#ifdef MU_LOG_ENABLE_FORMATTED
int my_custom_log_output(mu_log_level_t level, const char *format, va_list ap) {
    // Example: prefix with level name and forward to a hypothetical serial printf
    // Be mindful of reentrancy if called from ISRs

    const char *level_name = mu_log_level_name(level); // Get level string name

    // --- Your platform-specific output code here ---
    // Example using hypothetical serial functions:
    // serial_printf("%s: ", level_name);
    // serial_vprintf(format, ap);
    // serial_printf("\n");
    // ---------------------------------------------

    // Return number of bytes written (approximate if needed)
    return 0;
}
#else // Logging without printf-style formatting using simple strings:
int my_custom_log_output(mu_log_level_t level, const char *message) {
    // Example: prefix with level name and forward to a hypothetical serial puts
    const char *level_name = mu_log_level_name(level);

    // --- Your platform-specific output code here ---
    // Example using hypothetical serial functions:
    // serial_puts(level_name);
    // serial_puts(": ");
    // serial_puts(message);
    // serial_puts("\n");
    // ---------------------------------------------

    return 0; // Return number of bytes written (approximate if needed)
}
#endif

// In your application's initialization:
void app_init() {
    MU_LOG_SET_FN(my_custom_log_output);
    MU_LOG_SET_THRESHOLD(MU_LOG_LEVEL_DEBUG);

    MU_LOG_INFO("Logging initialized with custom output.");
    MU_LOG_DEBUG("Custom log message example.");
}
```
