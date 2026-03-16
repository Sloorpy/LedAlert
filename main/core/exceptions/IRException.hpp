#pragma once

enum class IRErrorCode {
    SUCCESS = 0,
    ENABLE_CHANNEL_FAILED = 1,
    TX_CHANNEL_CREATE_FAILED = 2,
    RX_CHANNEL_CREATE_FAILED = 3,
    RX_CALLBACK_REGISTER_FAILED = 4,
    RMT_RECEIVE_FAILED = 5,
    NOT_NEC_PROTOCOL = 7,
    ADDRESSES_DONT_MATCH = 8,
    COMMANDS_DONT_MATCH = 9,
    RMT_CHANNEL_WAS_FREED = 10
};

class IRException final
{
public:
    IRException(const IRErrorCode err) :
        _err_code(err)
    {   
    }

public:
    IRErrorCode get() const { return _err_code; }

private:
    const IRErrorCode _err_code;
};
