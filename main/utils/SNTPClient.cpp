#include "SNTPClient.hpp"

#include <ctime>
#include <cstdio>

#include <esp_netif_sntp.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static constexpr uint32_t RETRY_DELAY_MS = 1000;
static constexpr uint8_t MAX_RETRIES = 10;

SNTPClient::SNTPClient()
    : _initialized(false),
      _synced(false)
{
}

SNTPClient::~SNTPClient()
{
    try 
    {
        if (_initialized)
        {
            esp_netif_sntp_deinit();
            _initialized = false;
        }
    } 
    catch (...) {}
}

bool SNTPClient::is_time_valid() const
{
    time_t now = time(nullptr);
    return now > MIN_VALID_TIME;
}

bool SNTPClient::sync_time(const WifiSTA& wifi)
{
    if (!wifi.is_connected())
    {
        return false;
    }

    if (_synced)
    {
        printf("[Time] Already synced previously\n");
        return true;
    }

    return sync_time_impl(wifi);
}

bool SNTPClient::sync_time_impl(const WifiSTA& wifi)
{
    setenv("TZ", "IST-2IDT,M3.4.4/26,M10.5.0", 1);
    tzset();

    for (uint8_t attempt = 0; attempt < MAX_RETRIES; attempt++)
    {
        if (!wifi.is_connected())
        {
            printf("[Time] WiFi disconnected, aborting sync\n");
            return false;
        }

        printf("[Time] Sync attempt %u/%u\n", attempt + 1, MAX_RETRIES);

        if (try_sync())
        {
            _synced = true;
            return true;
        }

        if (attempt < MAX_RETRIES - 1)
        {
            vTaskDelay(pdMS_TO_TICKS(RETRY_DELAY_MS));
        }
    }

    printf("[Time] Sync failed after %u attempts\n", MAX_RETRIES);
    return false;
}

bool SNTPClient::try_sync()
{
    if (_initialized)
    {
        esp_netif_sntp_deinit();
    }

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("il.pool.ntp.org");

    esp_netif_sntp_init(&config);
    _initialized = true;

    static constexpr uint32_t SYNC_TIMEOUT_MS = 8000;
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(SYNC_TIMEOUT_MS)) == ESP_OK)
    {
        time_t now = time(nullptr);
        printf("[Time] Time synced successfully: %s", ctime(&now));
        return true;
    }

    return false;
}
