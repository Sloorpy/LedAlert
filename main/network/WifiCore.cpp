#include "WifiCore.hpp"
#include "Exception.hpp"
#include "EspException.hpp"

#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>

static constexpr int CONNECTED_BIT = BIT0;
static constexpr int FAILED_BIT = BIT1;
static constexpr int GOT_IP_BIT = BIT2;
static EventGroupHandle_t s_event_group = nullptr;

std::shared_ptr<WifiCore> WifiCore::s_instance = nullptr;

WifiCore::WifiCore()
{
}

WifiCore::~WifiCore()
{
    deinit();
}

std::shared_ptr<WifiCore> WifiCore::get_instance()
{
    if (!s_instance)
    {
        s_instance = std::shared_ptr<WifiCore>(new WifiCore());
    }
    return s_instance;
}

void WifiCore::init()
{
    if (_initialized)
    {
        return;
    }

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();

        if (ret != ESP_OK)
        {
            throw EspException(ret);
        }
    }

    ret = esp_netif_init();
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        throw EspException(ret);
    }

    s_event_group = xEventGroupCreate();

    const wifi_init_config_t wifi_conf = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&wifi_conf);
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    ret = esp_wifi_set_mode(WIFI_MODE_NULL);
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    ret = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiCore::wifi_event_handler, this);
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiCore::ip_event_handler, this);
    if (ret != ESP_OK)
    {
        throw EspException(ret);
    }

    _initialized = true;
    printf("[Wifi] Core initialized\n");
}

void WifiCore::deinit()
{
    if (!_initialized)
    {
        return;
    }

    esp_wifi_deinit();

    if (_sta_netif)
    {
        esp_netif_destroy_default_wifi(_sta_netif);
        _sta_netif = nullptr;
    }

    if (_ap_netif)
    {
        esp_netif_destroy_default_wifi(_ap_netif);
        _ap_netif = nullptr;
    }

    esp_event_loop_delete_default();
    esp_netif_deinit();

    if (s_event_group)
    {
        vEventGroupDelete(s_event_group);
        s_event_group = nullptr;
    }

    _initialized = false;
    _sta_connected = false;
    _is_connecting = false;
    printf("[Wifi] Core deinitialized\n");
}

void WifiCore::add_wifi_mode(const wifi_mode_t mode)
{
    if (mode != WIFI_MODE_STA && mode != WIFI_MODE_AP)
    {
        throw Exception(ErrorCode::INVALID_WIFI_MODE);
    }

    wifi_mode_t current_mode = WIFI_MODE_NULL;
    esp_err_t ret = esp_wifi_get_mode(&current_mode);
    if (ret != ESP_OK)
    {
        printf("[Wifi] Failed to get mode: %d\n", ret);
        throw Exception(ErrorCode::FAILED_TO_GET_WIFI_MODE);
    }

    wifi_mode_t new_mode = calc_add_mode(current_mode, mode);
    if (new_mode == current_mode)
    {
        return;
    }

    ret = esp_wifi_set_mode(new_mode);
    if (ret != ESP_OK)
    {
        printf("[Wifi] Failed to set mode: %d\n", ret);
        throw Exception(ErrorCode::FAILED_TO_SET_WIFI_MODE);
    }
}

wifi_mode_t WifiCore::calc_add_mode(wifi_mode_t current, wifi_mode_t to_add)
{
    if (to_add != WIFI_MODE_STA && to_add != WIFI_MODE_AP)
    {
        throw Exception(ErrorCode::INVALID_WIFI_MODE);
    }

    wifi_mode_t new_mode = to_add;

    switch (current)
    {
        case WIFI_MODE_NULL:
            break;
        case WIFI_MODE_STA:
            new_mode = (to_add == WIFI_MODE_STA) ? WIFI_MODE_STA : WIFI_MODE_APSTA;
            break;
        case WIFI_MODE_AP:
            new_mode = (to_add == WIFI_MODE_AP) ? WIFI_MODE_AP : WIFI_MODE_APSTA;
            break;
        case WIFI_MODE_APSTA:
            new_mode = WIFI_MODE_APSTA;
            break;
        default:
            throw Exception(ErrorCode::INVALID_WIFI_MODE);
    }

    return new_mode;
}

void WifiCore::remove_wifi_mode(const wifi_mode_t mode)
{
    if (mode != WIFI_MODE_STA && mode != WIFI_MODE_AP)
    {
        throw Exception(ErrorCode::INVALID_WIFI_MODE);
    }

    wifi_mode_t current_mode = WIFI_MODE_NULL;
    esp_err_t ret = esp_wifi_get_mode(&current_mode);
    if (ret != ESP_OK)
    {
        printf("[Wifi] Failed to get mode: %d\n", ret);
        throw Exception(ErrorCode::FAILED_TO_GET_WIFI_MODE);
    }

    wifi_mode_t new_mode = calc_remove_mode(current_mode, mode);
    if (new_mode == current_mode)
    {
        return;
    }

    ret = esp_wifi_set_mode(new_mode);
    if (ret != ESP_OK)
    {
        printf("[Wifi] Failed to set mode: %d\n", ret);
        throw Exception(ErrorCode::FAILED_TO_SET_WIFI_MODE);
    }
}

wifi_mode_t WifiCore::calc_remove_mode(wifi_mode_t current, wifi_mode_t to_remove)
{
    if (to_remove != WIFI_MODE_STA && to_remove != WIFI_MODE_AP)
    {
        throw Exception(ErrorCode::INVALID_WIFI_MODE);
    }

    wifi_mode_t new_mode = WIFI_MODE_NULL;
    switch (current)
    {
        case WIFI_MODE_NULL:
            break;
        case WIFI_MODE_STA:
            new_mode = (to_remove == WIFI_MODE_STA) ? WIFI_MODE_NULL : WIFI_MODE_STA;
            break;
        case WIFI_MODE_AP:
            new_mode = (to_remove == WIFI_MODE_AP) ? WIFI_MODE_NULL : WIFI_MODE_AP;
            break;
        case WIFI_MODE_APSTA:
            new_mode = (to_remove == WIFI_MODE_STA) ? WIFI_MODE_AP : WIFI_MODE_STA;
            break;
        default:
            throw Exception(ErrorCode::INVALID_WIFI_MODE);
    }

    return new_mode;
}

void WifiCore::set_sta_connected(bool connected)
{
    _sta_connected = connected;
}

void WifiCore::set_has_ip(bool has_ip)
{
    _has_ip = has_ip;
}

void WifiCore::notify_connected()
{
    if (s_event_group)
    {
        xEventGroupSetBits(s_event_group, GOT_IP_BIT);
    }
}

void WifiCore::notify_failed()
{
    if (s_event_group && _is_connecting)
    {
        xEventGroupSetBits(s_event_group, FAILED_BIT);
    }
}

bool WifiCore::wait_for_connection(uint32_t timeout_ms)
{
    int64_t start_time = esp_timer_get_time() / 1000;

    while (true)
    {
        EventBits_t bits = xEventGroupWaitBits(
            s_event_group,
            GOT_IP_BIT | FAILED_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(100)
        );

        if (bits & GOT_IP_BIT)
        {
            return true;
        }

        if (bits & FAILED_BIT)
        {
            return false;
        }

        if (timeout_ms > 0)
        {
            int64_t elapsed = (esp_timer_get_time() / 1000) - start_time;
            if (elapsed >= static_cast<int64_t>(timeout_ms))
            {
                printf("[Wifi] Connection timeout\n");
                return false;
            }
        }
    }
}

void WifiCore::clear_connection_events()
{
    if (s_event_group)
    {
        xEventGroupClearBits(s_event_group, GOT_IP_BIT | FAILED_BIT);
    }
}

esp_netif_t* WifiCore::create_sta_netif()
{
    if (_sta_netif == nullptr)
    {
        _sta_netif = esp_netif_create_default_wifi_sta();
    }
    return _sta_netif;
}

esp_netif_t* WifiCore::create_ap_netif()
{
    if (_ap_netif == nullptr)
    {
        _ap_netif = esp_netif_create_default_wifi_ap();
    }
    return _ap_netif;
}

esp_err_t WifiCore::get_sta_channel(uint8_t& channel) const
{
    wifi_ap_record_t ap_info = {};
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    if (ret != ESP_OK)
    {
        return ret;
    }
    channel = ap_info.primary;
    return ESP_OK;
}

void WifiCore::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (!arg)
    {
        return;
    }

    WifiCore* core = static_cast<WifiCore*>(arg);

    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
            case WIFI_EVENT_STA_START:
                printf("[Wifi] Event: WIFI_EVENT_STA_START\n");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                printf("[Wifi] Event: WIFI_EVENT_STA_CONNECTED\n");
                core->set_sta_connected(true);
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                printf("[Wifi] Event: WIFI_EVENT_STA_DISCONNECTED\n");
                core->set_sta_connected(false);
                core->set_has_ip(false);
                core->notify_failed();
                break;
        }
    }
}

void WifiCore::ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (!arg)
    {
        return;
    }

    WifiCore* core = static_cast<WifiCore*>(arg);

    if (event_base == IP_EVENT)
    {
        switch (event_id)
        {
            case IP_EVENT_STA_GOT_IP:
                printf("[Wifi] Event: IP_EVENT_STA_GOT_IP\n");
                core->set_has_ip(true);
                core->notify_connected();
                break;
            case IP_EVENT_STA_LOST_IP:
                printf("[Wifi] Event: IP_EVENT_STA_LOST_IP\n");
                core->set_has_ip(false);
                break;
        }
    }
}
