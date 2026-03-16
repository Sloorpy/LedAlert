#pragma once

#include "esp_err.h"

class EspException final
{
public:
    EspException(const esp_err_t err) :
        _err_code(err)
    {   
    }

public:
    esp_err_t get() const { return _err_code; }

private:
    const esp_err_t _err_code;
};
