#ifndef SCANNER_DEFINES_H
#define SCANNER_DEFINES_H

#define SCAN_SUCCESS 0
#define SCAN_ERROR_DIR_NOT_EXIST 1
#define SCAN_ERROR_CANT_READ_DIR 2

struct scan_results
{
    size_t file_counter;
    size_t js_counter;
    size_t unix_counter;
    size_t macOS_counter;
    size_t error_counter;
    size_t duration_s;
    size_t duration_ms;
    size_t duration_us;
};

#endif