#include "AlarmController.hpp"
#include "Log.hpp"

#include <ctime>
#include <cstring>

AlarmController::AlarmController(AlertsManager& alerts_manager, LedManager& led_manager) :
    _alerts_manager(alerts_manager),
    _led_manager(led_manager)
{
}

void AlarmController::start() {
    while (true) {
        check_and_play();
    }
}

void AlarmController::check_and_play() {
    AlarmType alert_type = _alerts_manager.check();

    //alert_type = AlarmType::AirRaid;
    switch (alert_type) {
        case AlarmType::AirRaid:
            LOGI("AlarmController", "Air raid alert detected!");
            play_alarm(AlarmType::AirRaid, 10);
            break;

        case AlarmType::Warning:
            LOGI("AlarmController", "Warning alert detected!");
            play_alarm(AlarmType::Warning, DEFAULT_DURATION_SEC);
            break;

        case AlarmType::None:
        default:
            sleep(POLL_INTERVAL_SEC);
            break;
    }
}

static constexpr uint64_t NS_PER_SEC = 1000000000ULL;
void AlarmController::play_alarm(AlarmType type, uint32_t duration_sec) {
    timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    const uint64_t alarm_duration_ns = duration_sec * NS_PER_SEC;

    uint64_t elapsed_ns = 0;
    while (elapsed_ns < alarm_duration_ns) {
        _led_manager.play(type);

        update_elapsed(elapsed_ns, start_time);
    }
}

void AlarmController::update_elapsed(uint64_t& elapsed_ns, const timespec& start_time)
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    elapsed_ns = (current_time.tv_sec - start_time.tv_sec) * NS_PER_SEC +
                                (current_time.tv_nsec - start_time.tv_nsec);
}

