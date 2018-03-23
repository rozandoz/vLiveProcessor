#pragma once

#include <iostream>

#include "i_logger_sink.h"

class ConsoleLoggerSink : public ILoggerSink
{
public:
    ConsoleLoggerSink()
    {
    }

    int Message(MessageType type, std::string msg) override 
    {
        std::cout << msg; return 0;
    }
};
