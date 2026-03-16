#pragma once

#include <cstdint>
#include <memory>

#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_netif.h>

#include "Macros.hpp"

class WifiCore final
{
public:
    static std::shared_ptr<WifiCore> get_instance();

    ~WifiCore();

public:
    DELETE_COPY_MOVE(WifiCore)

public:
    void init();
    void deinit();

    void set_sta_connected(bool connected);
    bool is_sta_connected() const { return _sta_connected; }

    void set_has_ip(bool has_ip);
    bool has_ip() const { return _has_ip; }

    void set_connecting(bool connecting) { _is_connecting = connecting; }
    bool is_connecting() const { return _is_connecting; }

    void add_wifi_mode(const wifi_mode_t mode);
    void remove_wifi_mode(const wifi_mode_t mode);

    void notify_connected();
    void notify_failed();
    bool wait_for_connection(uint32_t timeout_ms);
    void clear_connection_events();

    esp_netif_t* create_sta_netif();
    esp_netif_t* create_ap_netif();

    esp_err_t get_sta_channel(uint8_t& channel) const;

private:
    wifi_mode_t calc_add_mode(wifi_mode_t current, wifi_mode_t to_add);
    wifi_mode_t calc_remove_mode(wifi_mode_t current, wifi_mode_t to_remove);

private:
    WifiCore();
    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

private:

    static std::shared_ptr<WifiCore> s_instance;

    bool _initialized = false;
    bool _sta_connected = false;
    bool _has_ip = false;
    bool _is_connecting = false;

    esp_netif_t* _sta_netif = nullptr;
    esp_netif_t* _ap_netif = nullptr;
};
