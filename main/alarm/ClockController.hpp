#pragma once
#include "LedManager.hpp"

#include <unistd.h>

class ClockController final {
public:
    ClockController(LedManager& led_manager);
    ~ClockController() = default;

    DELETE_COPY_MOVE(ClockController)

    void start();

private:
    void check_and_play();
    void play_alarm(AlarmType type, uint32_t duration_sec);
    void update_elapsed(uint64_t& elapsed_ns, const timespec& start_time);

private:
    LedManager& _led_manager;

    static constexpr uint32_t DEFAULT_DURATION_SEC = 120;
};
