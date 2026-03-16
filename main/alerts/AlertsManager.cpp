#include "AlertsManager.hpp"

#include <ctime>

#include "json.hpp"

using nlohmann::json;

enum class LiveAlertCategory : uint8_t {
    AIR_RAID = 1,
    HOSTILE_UFO = 2,
    LIVE_HOSTILE_UFO = 6,
    HOSTILE_UFO_OVER = 10,
    ALERT_OVER = 13,
    WARNING = 14,
};

AlertsManager::AlertsManager(const std::string& city) :
    _city(city),
    _client()
{
}

AlarmType AlertsManager::check() {
    if (has_alert_live() || has_alert_history()) {
        return AlarmType::AirRaid;
    }

    return AlarmType::None;
}

bool AlertsManager::has_alert_live() {
    json result = _client.get_json(LIVE_ALERT);
    
    if (result.empty()) {
        return false;
    }

    if (!result.contains("cat") || !result["cat"].is_string()) {
        return false;
    }

    std::string category_str = result["cat"].get<std::string>();
    AlarmType alarm_type = map_category_to_alarm_type(category_str);

    if (alarm_type == AlarmType::None) {
        return false;
    }

    return find_city_in_data(result, _city);
}

bool AlertsManager::has_alert_history() {
    json latest_alert = _client.get_json_first(ALERT_HISTORY);

    if (latest_alert.empty()) {
        return false;
    }
    
    if (!find_city_in_data(latest_alert, _city)) {
        return false;
    }

    if (!latest_alert.contains("category") || !latest_alert["category"].is_number_integer()) {
        return false;
    }

    LiveAlertCategory category = static_cast<LiveAlertCategory>(latest_alert["category"].get<uint32_t>());
    if (category == LiveAlertCategory::ALERT_OVER || category == LiveAlertCategory::HOSTILE_UFO_OVER) {
        return false;
    }

    static constexpr std::string_view ALERT_DATE_FIELD = "alertDate";

    if (!latest_alert.contains(ALERT_DATE_FIELD) || !latest_alert[ALERT_DATE_FIELD].is_string()) {
        return false;
    }

    std::string alert_date_str = latest_alert[ALERT_DATE_FIELD].get<std::string>();

    int year, month, day, hour, minute, second;
    if (sscanf(alert_date_str.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second) != 6) {
        return false;
    }

    struct tm alert_tm = {};
    alert_tm.tm_year = year - TM_YEAR_OFFSET;
    alert_tm.tm_mon = month - 1 - TM_MONTH_OFFSET;
    alert_tm.tm_mday = day;
    alert_tm.tm_hour = hour;
    alert_tm.tm_min = minute;
    alert_tm.tm_sec = second;
    alert_tm.tm_isdst = TM_DST_AUTO_DETECT;

    time_t alert_time = mktime(&alert_tm);
    time_t now = time(nullptr);

    double diff = difftime(now, alert_time);

    return diff >= 0 && diff <= HISTORY_TIME_DIFF_SEC;
}

bool AlertsManager::find_city_in_data(const json& response, const std::string& city) const {
    static constexpr std::string_view CITIES = "data";

    if (!response.contains(CITIES)) {
        return false;
    }

    if (response[CITIES].is_string() && response[CITIES].get<std::string>() == city) {
        return true;
    }

    if (!response["data"].is_array()) {
        return false;
    }

    for (const auto& item : response["data"]) {
        if (!item.is_string()) {
            continue;
        }

        if (item.get<std::string>() == city) {
            return true;
        }
    }

    return false;
}

AlarmType AlertsManager::map_category_to_alarm_type(const std::string& category_str) const {
    try {
        int category_int = std::stoi(category_str);
        LiveAlertCategory category = static_cast<LiveAlertCategory>(category_int);

        switch (category) {
            case LiveAlertCategory::ALERT_OVER:
            case LiveAlertCategory::HOSTILE_UFO_OVER:
                return AlarmType::None;

            case LiveAlertCategory::AIR_RAID:
            case LiveAlertCategory::HOSTILE_UFO:
            case LiveAlertCategory::LIVE_HOSTILE_UFO:
                return AlarmType::AirRaid;

            case LiveAlertCategory::WARNING:
                return AlarmType::Warning;

            default:
                return AlarmType::None;
        }
    } catch (...) {
        return AlarmType::None;
    }
}
