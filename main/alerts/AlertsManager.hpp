#pragma once

#include <string>

#include "HTTPClient.hpp"
#include "AlarmType.hpp"

class AlertsManager final {
public:
    explicit AlertsManager(const std::string& city);
    ~AlertsManager() = default;

    DELETE_COPY_MOVE(AlertsManager)

    AlarmType check();

private:
    bool has_alert_live();
    bool has_alert_history();
    bool find_city_in_data(const json& response, const std::string& city) const;
    AlarmType map_category_to_alarm_type(const std::string& category_str) const;

private:
    static constexpr std::string_view LIVE_ALERT = "https://www.oref.org.il/warningMessages/alert/Alerts.json";
    static constexpr std::string_view ALERT_HISTORY = "https://alerts-history.oref.org.il//Shared/Ajax/GetAlarmsHistory.aspx?lang=he&mode=1&city_0=%D7%A0%D7%AA%D7%A0%D7%99%D7%94%20-%20%D7%9E%D7%A2%D7%A8%D7%91";
    static constexpr uint32_t HISTORY_TIME_DIFF_SEC = 3 * 60;

    // C 'struct tm' stores year as years since 1900 (e.g., 2026 -> 126)
    static constexpr int TM_YEAR_OFFSET = 1900;
    // C 'struct tm' stores months as 0-11 (January=0, December=11)
    static constexpr int TM_MONTH_OFFSET = 0;
    // Let mktime() auto-detect daylight saving time
    static constexpr int TM_DST_AUTO_DETECT = -1;

private:
    const std::string _city;
    HTTPClient _client;
};
