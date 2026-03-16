#include "WifiSTA.hpp"
#include "WifiCore.hpp"
#include "Exception.hpp"
#include "Log.hpp"

#include <cstring>

uint8_t WifiSTA::s_channel = DEFAULT_CHANNEL;

WifiSTA::WifiSTA(const std::string_view& ssid, const std::string_view& password) :
    _core(WifiCore::get_instance()),
    _ssid(ssid.data(), ssid.size()),
    _password(password.data(), password.size())
{
    _core->init();
}

WifiSTA::~WifiSTA()
{   
    try 
    {
        if (!_core->is_sta_connected())
        {
            return;
        }

        esp_wifi_disconnect();
    }
    catch(...) {}

    try 
    {
        _core->set_sta_connected(false);
        _core->remove_wifi_mode(WIFI_MODE_STA);  
    }
    catch(...) {}
}

wifi_config_t WifiSTA::create_wifi_config() const
{
    wifi_sta_config_t sta_conf = {};
    std::memcpy(sta_conf.ssid, _ssid.data(), _ssid.size());
    sta_conf.ssid[_ssid.size()] = '\0';
    std::memcpy(sta_conf.password, _password.data(), _password.size());
    sta_conf.password[_password.size()] = '\0';
    sta_conf.scan_method = WIFI_FAST_SCAN;

    if (s_channel != DEFAULT_CHANNEL)
    {
        sta_conf.channel = s_channel;
        LOGI("Wifi", "Using stored channel %d", s_channel);
    }

   return wifi_config_t{.sta = sta_conf};
}

void WifiSTA::store_connection_info()
{
    uint8_t channel = DEFAULT_CHANNEL;
    esp_err_t ret = _core->get_sta_channel(channel);
    if (ret == ESP_OK)
    {
        s_channel = channel;
    }
}

void WifiSTA::connect(uint32_t timeout_ms)
{
    if (_core->has_ip() && _core->is_sta_connected())
    {
        LOGI("Wifi", "Already connected");
        return;
    }

    _core->clear_connection_events();
    _core->create_sta_netif();
    _core->add_wifi_mode(WIFI_MODE_STA);

    wifi_config_t wifi_conf = create_wifi_config();
    esp_err_t ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_conf);
    if (ret != ESP_OK)
    {
        LOGE("Wifi", "Failed to set config: %d", ret);
        throw Exception(ErrorCode::FAILED_TO_SET_WIFI_CONFIG);
    }

    ret = esp_wifi_start();
    if (ret != ESP_OK && ret != ESP_ERR_WIFI_STATE)
    {
        LOGE("Wifi", "Failed to start WiFi: %d", ret);
        throw Exception(ErrorCode::FAILED_TO_START_WIFI);
    }

    _core->set_connecting(true);
    _core->set_sta_connected(false);

    ret = esp_wifi_connect();
    if (ret != ESP_OK)
    {
        LOGE("Wifi", "Failed to connect: %d", ret);
        _core->set_connecting(false);
        throw Exception(ErrorCode::FAILED_TO_CALL_WIFI_CONNECT);
    }

    bool connected = _core->wait_for_connection(timeout_ms);
    _core->set_connecting(false);

    if (connected)
    {
        LOGI("Wifi", "Connected!");
        store_connection_info();
        return;
    }

    LOGI("Wifi", "Connection failed");
}

bool WifiSTA::is_connected() const
{
    return _core->has_ip();
}

void WifiSTA::disconnect()
{
    if (_core->is_sta_connected())
    {
        esp_wifi_disconnect();
        _core->set_sta_connected(false);
        _core->set_has_ip(false);
        LOGI("Wifi", "Disconnected");
    }
}
