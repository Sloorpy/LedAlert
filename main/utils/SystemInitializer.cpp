#include "SystemInitializer.hpp"
#include "Log.hpp"

#include <cinttypes>
#include <unistd.h>

std::unique_ptr<WifiSTA> SystemInitializer::create_wifi(std::string_view ssid, std::string_view password) {
    return std::make_unique<WifiSTA>(ssid, password);
}

std::unique_ptr<SNTPClient> SystemInitializer::create_sntp() {
    return std::make_unique<SNTPClient>();
}

bool SystemInitializer::connect_wifi(WifiSTA& wifi) {
    try 
    {
        LOGI("WiFi", "Connecting...");
        wifi.connect();

        return wifi.is_connected();
    }
    catch (...)
    {
        return false;
    }
}

bool SystemInitializer::sync_time(SNTPClient& sntp, WifiSTA& wifi) {
    try 
    {
        LOGI("Time", "Syncing time...");
        return sntp.sync_time(wifi);
    }
    catch(...)
    {
        return false;
    }
}
