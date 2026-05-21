#pragma once

#include <cstdio>
#include <ctime>
#include <esp_log.h>
#include <esp_debug_helpers.h>

static constexpr time_t MIN_VALID_TIME = 100000;

inline void get_timestamp(char* buffer, size_t size) {
    time_t now = time(nullptr);
    
    if (now > MIN_VALID_TIME) {
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        strftime(buffer, size, "%H:%M:%S-%d/%m/%y", &timeinfo);
    } else {
        snprintf(buffer, size, "~%06u", (unsigned int)esp_log_timestamp());
    }
}

#define LOGI(tag, fmt, ...) do { \
    char ts[24]; \
    get_timestamp(ts, sizeof(ts)); \
    ESP_LOGI(tag, "[%s] " tag ": " fmt, ts, ##__VA_ARGS__); \
} while(0)

#define LOGW(tag, fmt, ...) do { \
    char ts[24]; \
    get_timestamp(ts, sizeof(ts)); \
    ESP_LOGW(tag, "[%s] " tag ": " fmt, ts, ##__VA_ARGS__); \
} while(0)

#define LOGE(tag, fmt, ...) do { \
    char ts[24]; \
    get_timestamp(ts, sizeof(ts)); \
    ESP_LOGE(tag, "[%s] " tag ": " fmt, ts, ##__VA_ARGS__); \
} while(0)

#define LOGE_CATCH(tag, msg) do { \
    char ts[24]; \
    get_timestamp(ts, sizeof(ts)); \
    ESP_LOGE(tag, "[%s] " tag ": \033[1;33mERROR: %s\033[0m", ts, msg); \
    esp_backtrace_print(10); \
} while(0)
