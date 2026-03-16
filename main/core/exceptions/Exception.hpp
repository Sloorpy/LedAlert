#pragma once

enum class ErrorCode {
    SUCCESS = 0,
    INDEX_OUT_OF_RANGE = 1,
    XQUEUE_CREATE_FAILED = 2,
    FAILED_TO_SET_STATION_MODE = 3,
    INVALID_WIFI_MODE = 4,
    FAILED_TO_GET_WIFI_MODE = 5,
    FAILED_TO_SET_WIFI_MODE = 6,
    FAILED_TO_CALL_WIFI_CONNECT = 7,
    FAILED_TO_START_WIFI = 8,
    FAILED_TO_SET_WIFI_CONFIG = 9,
};

class Exception final
{
public:
    Exception(const ErrorCode err) :
        _err_code(err)
    {   
    }

public:
    ErrorCode get() const { return _err_code; }

private:
    const ErrorCode _err_code;
};
