#pragma once

#include <unistd.h>

#include "AlertsManager.hpp"
#include "LedManager.hpp"

class AlarmController final {
public:
    AlarmController(AlertsManager& alerts_manager, LedManager& led_manager);
    ~AlarmController() = default;

    DELETE_COPY_MOVE(AlarmController)

    void start();

private:
    void check_and_play();
    void play_alarm(AlarmType type, uint32_t duration_sec);
    void update_elapsed(uint64_t& elapsed_ns, const timespec& start_time);

private:
    AlertsManager& _alerts_manager;
    LedManager& _led_manager;

    static constexpr uint32_t DEFAULT_DURATION_SEC = 120;
    static constexpr uint32_t POLL_INTERVAL_SEC = 1;
};
