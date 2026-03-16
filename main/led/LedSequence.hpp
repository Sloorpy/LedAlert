#pragma once

#include "IRCommand.hpp"

struct LedAction{
    IRCommand cmd;
    uint32_t duration_ms;
};

class LedSequence final 
{
public:
    LedSequence() = default;
    explicit LedSequence(const std::vector<LedAction>& led_sequence);
    ~LedSequence() = default;

public:
    const std::vector<LedAction>& get_sequence() const;
    void add_action(const LedAction& action);

private:
    std::vector<LedAction> _sequence;
};