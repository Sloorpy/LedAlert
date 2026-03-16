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
    for (const auto& action : sequence) {
        _transmitter->send_multiple(static_cast<LEDProtocol>(action.cmd.command));
        usleep(action.duration_ms * 1000);
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

    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::POWER_ON)}, .duration_ms = 10});
    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::BRIGHT_UP)}, .duration_ms = 10});
    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::RED_LIGHT)}, .duration_ms = 200});
    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::WHITE_LIGHT)}, .duration_ms = 200});
    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::GREEN_LIGHT)}, .duration_ms = 200});

    return LedSequence(actions);
}

LedSequence LedManager::create_warning_sequence() const {
    std::vector<LedAction> actions;

    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::POWER_ON)}, .duration_ms = 400});
    actions.push_back({.cmd = {.state = SignalState::NORMAL, .address = 0, .command = static_cast<uint8_t>(LEDProtocol::POWER_OFF)}, .duration_ms = 400});

    return LedSequence(actions);
}
