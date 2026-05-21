#include "LedSequence.hpp"

LedSequence::LedSequence(const std::vector<LedAction>& led_sequence) :
    _start_sequence(),
    _sequence(led_sequence)
{

}

LedSequence::LedSequence(const std::vector<LedAction> &led_sequence, const std::vector<LedAction> &start_sequence) :
    _start_sequence(start_sequence),
    _sequence(led_sequence)
{
}


const std::vector<LedAction> &LedSequence::get_sequence() const
{
    return _sequence;
}

const std::vector<LedAction> &LedSequence::get_start_sequence() const
{
    return _start_sequence;
}

void LedSequence::add_action(const LedAction& action)
{
    _sequence.emplace_back(action);
}

void LedSequence::add_start_action(const LedAction &action)
{
    _start_sequence.emplace_back(action);
}
