/**
 * @file mu_log.h
 * @brief Logging framework with configurable formatted or simple string output.
 * 
 * This module provides a global logger with configurable behavior. Users can
 * enable simple logging (plain string output) or formatted logging (printf-style).
 * 
 * **Usage:**
 * - Define `MU_LOG_ENABLE` to use simple string logging.
 * - Define `MU_LOG_ENABLE_FORMATTED` to enable printf-style formatted logging.
 * - If neither is defined, logging is disabled entirely.
 * 
 * The framework allows customization of the logging function (`mu_log_set_fn`)
 * and logging level (`mu_log_set_threshold`). When disabled, macros resolve to no-ops.
 */

#ifndef _MU_LOG_H_
#define _MU_LOG_H_

// *****************************************************************************
// Includes

#include <stdbool.h>

// *****************************************************************************
// C++ Compatibility

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MU_LOG_ENABLE) || defined(MU_LOG_ENABLE_FORMATTED) // (almost) whole file

// *****************************************************************************
// Public types and definitions

/**
 * @enum mu_log_level_t
 * @brief Defines log severity levels.
 */
#define MU_LOG_LEVELS(M)                                                       \
    M(MU_LOG_LEVEL_TRACE, "TRACE")                                             \
    M(MU_LOG_LEVEL_DEBUG, "DEBUG")                                             \
    M(MU_LOG_LEVEL_INFO,  "INFO")                                              \
    M(MU_LOG_LEVEL_WARN,  "WARN")                                              \
    M(MU_LOG_LEVEL_ERROR, "ERROR")                                             \
    M(MU_LOG_LEVEL_FATAL, "FATAL")

#define EXPAND_LOG_LEVEL_ENUM(_enum_id, _name) _enum_id,
typedef enum {
    MU_LOG_LEVELS(EXPAND_LOG_LEVEL_ENUM) /**< Enumeration of log levels */
} mu_log_level_t;

#define MU_LOG_DEFAULT_LEVEL MU_LOG_LEVEL_INFO /**< Default log level */

/**
 * @typedef mu_log_fn
 * @brief Function pointer type for logging output.
 *
 * Defines the function signature for logging, either in formatted or simple form.
 */
#ifdef MU_LOG_ENABLE_FORMATTED
#include <stdarg.h>
typedef int (*mu_log_fn)(mu_log_level_t level, const char *format, va_list ap);
#else
typedef int (*mu_log_fn)(mu_log_level_t level, const char *message);
#endif

/**
 * @struct mu_log_t
 * @brief Represents the global logger instance.
 */
typedef struct {
    mu_log_fn log_fn;          /**< User-defined logging function */
    mu_log_level_t threshold;  /**< Minimum severity level */
} mu_log_t;

// *****************************************************************************
// Public declarations

/**
 * @brief Sets the logging function.
 * 
 * @param[in] fn Pointer to the logging function.
 */
void mu_log_set_fn(mu_log_fn fn);

/**
 * @brief Gets the currently set logging function.
 * 
 * @return The current logging function pointer.
 */
mu_log_fn mu_log_get_fn(void);

/**
 * @brief Sets the minimum severity level for logging.
 * 
 * @param[in] level The new logging threshold level.
 */
void mu_log_set_threshold(mu_log_level_t level);

/**
 * @brief Gets the current logging threshold level.
 * 
 * @return The current threshold level.
 */
mu_log_level_t mu_log_get_threshold(void);

/**
 * @brief Logs a message if its level meets or exceeds the threshold.
 * 
 * @param[in] level Log severity level.
 * @param[in] format Format string (for formatted logging) or message string.
 * @param[in] ... Optional additional parameters for formatted logging.
 */
void mu_log(mu_log_level_t level,
  #ifdef MU_LOG_ENABLE_FORMATTED
    const char *format, ...
  #else
    const char *message
  #endif
  );

/**
 * @brief Determines if a message at the given level would be logged.
 * 
 * @param[in] level Log severity level.
 * @return `true` if the message would be logged, `false` otherwise.
 */
bool mu_log_will_log(mu_log_level_t level);

/**
 * @brief Gets the human-readable name of a log level.
 * 
 * @param[in] level Log severity level.
 * @return Corresponding string representation of the level.
 */
const char *mu_log_level_name(mu_log_level_t level);

/**
 * @brief A default logging function that prints to stdout.
 * 
 * Prints the invoked logging level, the message, and a newline:
 * `INFO: ... \n`
 * 
 * - If `MU_LOG_ENABLE_FORMATTED` is set, uses printf-style formatting.
 * - Otherwise, uses `fputs()`.
 * 
 * @param[in] level Log severity level.
 * @param[in] format Format string (if formatted logging is enabled) or message.
 * @param[in] ap Argument list for formatted logging.
 * @return Number of characters printed.
 */
int mu_log_stdout_fn(mu_log_level_t level, 
  #ifdef MU_LOG_ENABLE_FORMATTED
    const char *format, va_list ap
  #else
    const char *message
  #endif
);

// *****************************************************************************
// Logging Macros

#define MU_LOG_SET_FN(fn) mu_log_set_fn(fn) /**< Sets the log function */
#define MU_LOG_GET_FN() mu_log_get_fn() /**< Gets the log function */
#define MU_LOG_SET_THRESHOLD(level) mu_log_set_threshold(level) /**< Sets log level */
#define MU_LOG_GET_THRESHOLD() mu_log_get_threshold() /**< Gets log level */
#define MU_LOG(level, ...) mu_log(level, __VA_ARGS__) /**< Logs message */
#define MU_LOG_TRACE(...) mu_log(MU_LOG_LEVEL_TRACE, __VA_ARGS__) /**< Trace log */
#define MU_LOG_DEBUG(...) mu_log(MU_LOG_LEVEL_DEBUG, __VA_ARGS__) /**< Debug log */
#define MU_LOG_INFO(...)  mu_log(MU_LOG_LEVEL_INFO, __VA_ARGS__) /**< Info log */
#define MU_LOG_WARN(...)  mu_log(MU_LOG_LEVEL_WARN, __VA_ARGS__) /**< Warning log */
#define MU_LOG_ERROR(...) mu_log(MU_LOG_LEVEL_ERROR, __VA_ARGS__) /**< Error log */
#define MU_LOG_FATAL(...) mu_log(MU_LOG_LEVEL_FATAL, __VA_ARGS__) /**< Fatal log */
#define MU_LOG_WILL_LOG(level) mu_log_will_log(level) /**< Check if logging is enabled */
#define MU_LOG_LEVEL_NAME(level) mu_log_level_name(level) /**< Get level name */

#else

/** No-op macros when logging is disabled */
#define MU_LOG_SET_FN(fn) ((void)0)
#define MU_LOG_GET_FN() ((void)0)
#define MU_LOG_SET_THRESHOLD(level) ((void)0)
#define MU_LOG_GET_THRESHOLD() (0)
#define MU_LOG(level, ...) ((void)0)
#define MU_LOG_TRACE(...) ((void)0)
#define MU_LOG_DEBUG(...) ((void)0)
#define MU_LOG_INFO(...)  ((void)0)
#define MU_LOG_WARN(...)  ((void)0)
#define MU_LOG_ERROR(...) ((void)0)
#define MU_LOG_FATAL(...) ((void)0)
#define MU_LOG_WILL_LOG(level) (0)

#endif  /**< End of MU_LOG_ENABLE or MU_LOG_ENABLE_FORMATTED */

// *****************************************************************************
// End of file

#ifdef __cplusplus
}
#endif

#endif /* _MU_LOG_H_ */
