#pragma once

#include <ctime>
#include <memory>

#include <esp_netif_sntp.h>

#include "Macros.hpp"
#include "WifiSTA.hpp"

class SNTPClient final
{
public:
    SNTPClient();
    ~SNTPClient();

    DELETE_COPY_MOVE(SNTPClient)

    bool sync_time(const WifiSTA& wifi);

private:
    static constexpr time_t MIN_VALID_TIME = 0;

    bool is_time_valid() const;
    bool sync_time_impl(const WifiSTA& wifi);
    bool try_sync();
    bool _initialized = false;
    bool _synced = false;
};
