#include "LedSequence.hpp"

LedSequence::LedSequence(const std::vector<LedAction>& led_sequence) :
    _sequence(led_sequence)
{

}

const std::vector<LedAction>& LedSequence::get_sequence() const
{
    return _sequence;
}

void LedSequence::add_action(const LedAction& action)
{
    _sequence.emplace_back(action);
}
