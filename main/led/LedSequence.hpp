#pragma once

#include "IRCommand.hpp"
#include "LEDTransmitter.hpp"

struct LedAction{
    LEDProtocol cmd;
    uint32_t duration_ms;
    uint32_t amount = 1;
};

class LedSequence final 
{
public:
    LedSequence() = default;
    explicit LedSequence(const std::vector<LedAction>& led_sequence);
    explicit LedSequence(const std::vector<LedAction>& led_sequence, const std::vector<LedAction>& start_sequence);
    ~LedSequence() = default;

public:
    const std::vector<LedAction>& get_sequence() const;
    const std::vector<LedAction>& get_start_sequence() const;

    void add_action(const LedAction& action);
    void add_start_action(const LedAction& action);

private:
    std::vector<LedAction> _start_sequence;
    std::vector<LedAction> _sequence;
};