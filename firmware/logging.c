/**
 * Logging Module Implementation
 * 
 * Ring buffer based logging system for device debugging.
 */

#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "pico/stdlib.h"

// Ring buffer for log storage
static char log_buffer[LOG_BUFFER_SIZE];
static uint16_t log_head = 0;  // Write position
static uint16_t log_tail = 0;  // Read position
static uint16_t log_count = 0; // Number of entries
static bool log_overflow = false;
static log_level_t current_log_level = LOG_LEVEL_DEBUG;

// Level names for output
static const char *level_names[] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

void logging_init(void) {
    log_head = 0;
    log_tail = 0;
    log_count = 0;
    log_overflow = false;
    current_log_level = LOG_LEVEL_DEBUG;
    memset(log_buffer, 0, LOG_BUFFER_SIZE);
}

void logging_log(log_level_t level, const char *format, ...) {
    // Check log level filter
    if (level < current_log_level) {
        return;
    }
    
    // Validate level - ensure it's within bounds for level_names array
    if (level > LOG_LEVEL_ERROR) {
        level = LOG_LEVEL_ERROR;
    }
    
    // Format the log entry
    char entry[LOG_ENTRY_MAX_LEN];
    uint32_t timestamp = to_ms_since_boot(get_absolute_time());
    
    // Create header with timestamp and level (level is guaranteed valid here)
    int header_len = snprintf(entry, sizeof(entry), "[%lu][%s] ", 
                              (unsigned long)timestamp, level_names[level]);
    
    if (header_len < 0 || (size_t)header_len >= sizeof(entry)) {
        return;
    }
    
    // Format user message
    va_list args;
    va_start(args, format);
    int msg_len = vsnprintf(entry + header_len, sizeof(entry) - header_len - 1, format, args);
    va_end(args);
    
    if (msg_len < 0) {
        return;
    }
    
    // Calculate total length (cap to max entry length - 1 for newline)
    size_t total_len = (size_t)header_len + (size_t)msg_len;
    if (total_len >= sizeof(entry) - 1) {
        total_len = sizeof(entry) - 2;
    }
    
    // Add newline
    entry[total_len] = '\n';
    entry[total_len + 1] = '\0';
    total_len++;
    
    // Write to ring buffer
    for (size_t i = 0; i < total_len; i++) {
        log_buffer[log_head] = entry[i];
        log_head = (log_head + 1) % LOG_BUFFER_SIZE;
        
        // Check for overflow
        if (log_head == log_tail) {
            // Buffer full, advance tail to make room
            log_tail = (log_tail + 1) % LOG_BUFFER_SIZE;
            log_overflow = true;
            // Decrement count since we're losing an entry
            if (log_count > 0) {
                log_count--;
            }
        }
    }
    
    log_count++;
}

void logging_set_level(log_level_t level) {
    if (level > LOG_LEVEL_ERROR) {
        level = LOG_LEVEL_ERROR;
    }
    current_log_level = level;
}

log_level_t logging_get_level(void) {
    return current_log_level;
}

uint16_t logging_get_logs(char *buffer, uint16_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return 0;
    }
    
    uint16_t bytes_written = 0;
    uint16_t read_pos = log_tail;
    
    // Copy all data from tail to head
    while (read_pos != log_head && bytes_written < buffer_size - 1) {
        buffer[bytes_written++] = log_buffer[read_pos];
        read_pos = (read_pos + 1) % LOG_BUFFER_SIZE;
    }
    
    buffer[bytes_written] = '\0';
    return bytes_written;
}

uint16_t logging_get_count(void) {
    return log_count;
}

void logging_clear(void) {
    log_head = 0;
    log_tail = 0;
    log_count = 0;
    log_overflow = false;
    memset(log_buffer, 0, LOG_BUFFER_SIZE);
}

bool logging_has_overflow(void) {
    return log_overflow;
}
