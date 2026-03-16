#include "WifiAP.hpp"

#include "WifiCore.hpp"
#include "EspException.hpp"

#include <cstring>
#include <stdio.h>

WifiAP::WifiAP()
    : _core(WifiCore::get_instance())
{
    _core->init();
}

WifiAP::~WifiAP()
{
    stop();
}

void WifiAP::start(std::string_view ssid)
{
    if (_started)
    {
        printf("[Wifi] AP already started\n");
        return;
    }

    _core->create_ap_netif();

    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.ap.ssid, ssid.data(), sizeof(wifi_config.ap.ssid) - 1);
    wifi_config.ap.ssid_len = ssid.length();
    wifi_config.ap.channel = 1;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;

    wifi_mode_t current_mode = WIFI_MODE_NULL;
    esp_wifi_get_mode(&current_mode);

    wifi_mode_t new_mode = static_cast<wifi_mode_t>(current_mode | WIFI_MODE_AP);
    esp_err_t ret = esp_wifi_set_mode(new_mode);
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    _started = true;
    printf("[Wifi] AP started: %.*s\n", ssid.length(), ssid.data());
}

void WifiAP::stop()
{
    if (!_started)
    {
        return;
    }

    esp_wifi_stop();
    _started = false;
    printf("[Wifi] AP stopped\n");
}
