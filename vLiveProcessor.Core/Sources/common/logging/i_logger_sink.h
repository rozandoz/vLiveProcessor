#pragma once

#include <string>

enum MessageType
{
    Error = 0,
    Warning,
    Trace
};

class ILoggerSink
{
public:
    ILoggerSink() { }
    virtual ~ILoggerSink() { }

    virtual int Message(MessageType type, std::string msg) = 0;
};

