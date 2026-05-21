#include "ClockController.hpp"

ClockController::ClockController(LedManager &led_manager) :
    _led_manager(led_manager)
{
}

void ClockController::start()
{
    while (true) {
        check_and_play();
    }
}
