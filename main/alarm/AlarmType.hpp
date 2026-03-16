#pragma once

#include <cstdint>

enum class AlarmType : uint8_t
{
    None    = 0,
    AirRaid = 1,
    Warning = 2,
};
