#pragma once

#include <memory>
#include <string_view>

#include <driver/gpio.h>

#include "WifiSTA.hpp"
#include "SNTPClient.hpp"

class SystemInitializer {
public:
    static constexpr uint32_t DEFAULT_RETRY_DELAY_SEC = 5;

    SystemInitializer() = delete;

    static std::unique_ptr<WifiSTA> create_wifi(std::string_view ssid, std::string_view password);
    static std::unique_ptr<SNTPClient> create_sntp();

    static bool connect_wifi(WifiSTA& wifi);
    static bool sync_time(SNTPClient& sntp, WifiSTA& wifi);
};
