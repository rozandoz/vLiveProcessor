#pragma once

#include "i_logger_sink.h"
#include "logger_streams.h"

#include <vector>
#include <memory>
#include <mutex>

class Logger : ILoggerSink
{
public:
    static Logger& GetInstance();

private:
    Logger();
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

private:
    int Message(MessageType type, std::string msg) override;

public:
    void AddSink(std::shared_ptr<ILoggerSink> sink);

    //streams
    logger_stream error;
    logger_stream warning;
    logger_stream trace;

private:
    std::function<void(void)>                   m_guard;
    std::vector<std::shared_ptr<ILoggerSink>>   m_sinks;
    std::mutex                                  m_mutex;
};
