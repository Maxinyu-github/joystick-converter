/**
 * Logging Module
 * 
 * Provides logging functionality with ring buffer storage.
 * Logs can be uploaded to the PC configuration tool for debugging.
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <stdbool.h>
#include <stdint.h>

// Maximum size of log buffer (in bytes)
#define LOG_BUFFER_SIZE 4096

// Maximum length of a single log entry
#define LOG_ENTRY_MAX_LEN 128

// Log levels
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3
} log_level_t;

/**
 * Initialize logging system
 */
void logging_init(void);

/**
 * Log a message at the specified level
 * @param level Log level
 * @param format Printf-style format string
 * @param ... Format arguments
 */
void logging_log(log_level_t level, const char *format, ...);

/**
 * Convenience macros for logging at different levels
 */
#define LOG_DEBUG(fmt, ...) logging_log(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  logging_log(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  logging_log(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logging_log(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

/**
 * Set minimum log level to capture
 * @param level Minimum log level
 */
void logging_set_level(log_level_t level);

/**
 * Get current log level
 * @return Current minimum log level
 */
log_level_t logging_get_level(void);

/**
 * Get all logs as a formatted string
 * Caller must provide buffer for output
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return Number of bytes written
 */
uint16_t logging_get_logs(char *buffer, uint16_t buffer_size);

/**
 * Get number of log entries available
 * @return Number of log entries
 */
uint16_t logging_get_count(void);

/**
 * Clear all logs
 */
void logging_clear(void);

/**
 * Check if log buffer has overflowed (lost entries)
 * @return true if overflow occurred
 */
bool logging_has_overflow(void);

#endif // LOGGING_H
