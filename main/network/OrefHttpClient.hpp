#pragma once

#include "HTTPClient.hpp"
#include "Macros.hpp"

#include <string>
#include <string_view>

using nlohmann::json;

class OrefHttpClient final
{
public:
    OrefHttpClient();
    ~OrefHttpClient();

    DELETE_COPY_MOVE(OrefHttpClient)

    json get_alert();
    json get_history();

private:
    static constexpr std::string_view HOST_ALERT = "www.oref.org.il";
    static constexpr std::string_view HOST_HISTORY = "alerts-history.oref.org.il";
    static constexpr std::string_view PATH_ALERT = "/warningMessages/alert/Alerts.json";
    static constexpr std::string_view PATH_HISTORY = "/Shared/Ajax/GetAlarmsHistory.aspx?lang=he&mode=1&city_0=%D7%A0%D7%AA%D7%A0%D7%99%D7%94%20-%20%D7%9E%D7%A2%D7%A8%D7%91";

    HTTPClient _client;
};
