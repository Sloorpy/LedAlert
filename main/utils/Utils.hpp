#pragma once

#include <ctime>
#include <cstdio>

#include <esp_netif_sntp.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "WifiSTA.hpp"

namespace Utils
{
    bool sync_time(WifiSTA& wifi);
}
