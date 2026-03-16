#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <string>

#include <esp_err.h>
#include <esp_wifi.h>

#include "Macros.hpp"

class WifiCore;

static constexpr uint32_t DEFAULT_CONNECTION_TIMEOUT_MS = 10 * 1000;
static constexpr uint8_t DEFAULT_CHANNEL = 0;

class WifiSTA final
{
public:
    explicit WifiSTA(const std::string_view& ssid, const std::string_view& password);
    ~WifiSTA();

public:
    DELETE_COPY_MOVE(WifiSTA)

public:
    void connect(uint32_t timeout_ms = DEFAULT_CONNECTION_TIMEOUT_MS);
    void disconnect();
    bool is_connected() const;

private:
    wifi_config_t create_wifi_config() const;
    void store_connection_info();

private:
    std::shared_ptr<WifiCore> _core;

    const std::string _ssid;
    const std::string _password;

private:
    static uint8_t s_channel;
};
