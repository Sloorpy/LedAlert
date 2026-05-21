#include "OrefHttpClient.hpp"
#include "Log.hpp"
#include "EspException.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

OrefHttpClient::OrefHttpClient()
{
}

OrefHttpClient::~OrefHttpClient()
{
}

json OrefHttpClient::get_alert()
{
    std::string url = "https://" + std::string(HOST_ALERT) + PATH_ALERT.data();
    
    try
    {
        return _client.get_json(url);
    }
    catch (const EspException& ex)
    {
        LOGE("Request failed for %s: %d - %s", 
                HOST_ALERT.data(), ex.get(), esp_err_to_name(ex.get()));
    }
    catch (...)
    {
    }

    return json({});}

json OrefHttpClient::get_history()
{
    std::string url = "https://" + std::string(HOST_HISTORY) + PATH_HISTORY.data();
    
    try
    {
        return _client.get_json_first(url);
    }
    catch (const EspException& ex)
    {
        LOGE("Request failed for %s: %d - %s", 
                HOST_HISTORY.data(), ex.get(), esp_err_to_name(ex.get()));
    }
    catch (...)
    {
    }

    return json({});
}
