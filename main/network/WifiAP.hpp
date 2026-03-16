#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include <esp_err.h>
#include <esp_wifi.h>

#include "Macros.hpp"

class WifiCore;

class WifiAP final
{
public:
    explicit WifiAP();
    ~WifiAP();

    DELETE_COPY_MOVE(WifiAP)

    void start(std::string_view ssid);
    void stop();

    bool is_started() const { return _started; }

private:
    std::shared_ptr<WifiCore> _core;
    bool _started = false;
};
