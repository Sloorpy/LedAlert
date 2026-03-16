#pragma once
#include "LedSequence.hpp"
#include "AlarmType.hpp"
#include "Macros.hpp"
#include "LEDTransmitter.hpp"

#include <map>
#include <memory>
#include <driver/gpio.h>

class LedManager final {
public:
    explicit LedManager(gpio_num_t gpio);
    ~LedManager() = default;

    DELETE_COPY_MOVE(LedManager)

    void play(AlarmType type);

    void set_custom_sequence(AlarmType type, const LedSequence& seq);
    const LedSequence& get_sequence(AlarmType type) const;

private:
    void build_default_sequences();
    LedSequence create_air_raid_sequence() const;
    LedSequence create_warning_sequence() const;

private:
    const gpio_num_t _gpio;
    std::unique_ptr<LEDTransmitter> _transmitter;
    std::map<AlarmType, LedSequence> _sequences;
};
