#include "LedManager.hpp"

#include <unistd.h>

#include "LEDTransmitter.hpp"

LedManager::LedManager(gpio_num_t gpio) :
    _gpio(gpio),
    _transmitter(std::make_unique<LEDTransmitter>(gpio))
{
    build_default_sequences();
}

void LedManager::play(AlarmType type) {
    auto it = _sequences.find(type);
    if (it == _sequences.end()) {
        return;
    }

    const auto& sequence = it->second.get_sequence();
    for (const LedAction& action : sequence) {
        _transmitter->send_multiple(action.cmd, action.amount, action.duration_ms);
    }
}

void LedManager::set_custom_sequence(AlarmType type, const LedSequence& seq) {
    _sequences[type] = seq;
}

const LedSequence& LedManager::get_sequence(AlarmType type) const {
    static const LedSequence empty{};
    auto it = _sequences.find(type);
    if (it == _sequences.end()) {
        return empty;
    }
    return it->second;
}

void LedManager::build_default_sequences() {
    _sequences[AlarmType::AirRaid] = create_air_raid_sequence();
    _sequences[AlarmType::Warning] = create_warning_sequence();
}

LedSequence LedManager::create_air_raid_sequence() const {
    std::vector<LedAction> actions;
    std::vector<LedAction> start_actions;

    start_actions.push_back({.cmd = LEDProtocol::POWER_ON, .duration_ms = 10, .amount = 20});
    start_actions.push_back({.cmd = LEDProtocol::BRIGHT_UP, .duration_ms = 10, .amount = 10});


    actions.push_back({.cmd = LEDProtocol::RED_LIGHT, .duration_ms = 150});
    actions.push_back({.cmd = LEDProtocol::WHITE_LIGHT, .duration_ms = 150});
    actions.push_back({.cmd = LEDProtocol::GREEN_LIGHT, .duration_ms = 150});

    return LedSequence(actions, start_actions);
}

LedSequence LedManager::create_warning_sequence() const {
    std::vector<LedAction> actions;
    std::vector<LedAction> start_actions;

    start_actions.push_back({.cmd = LEDProtocol::POWER_ON, .duration_ms = 10});
    start_actions.push_back({.cmd = LEDProtocol::WHITE_LIGHT, .duration_ms = 10});
    start_actions.push_back({.cmd = LEDProtocol::BRIGHT_UP, .duration_ms = 10, .amount = 10});

    actions.push_back({.cmd = LEDProtocol::POWER_ON, .duration_ms = 1000});
    actions.push_back({.cmd = LEDProtocol::POWER_OFF, .duration_ms = 500});

    return LedSequence(actions, start_actions);
}
