#include "logger.h"

#include <cstdarg>

#include "common/strings.h"
#include "console_logger_sink.h"

using namespace std;

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : error([&](string msg) -> int { return Message(MessageType::Error, msg); })
    , warning([&](string msg) -> int { return Message(MessageType::Warning, msg); })
    , trace([&](string msg) -> int { return Message(MessageType::Trace, msg); })
{
    AddSink(make_shared<ConsoleLoggerSink>());
}

int Logger::Message(MessageType type, std::string msg)
{
    lock_guard<mutex> lock(m_mutex);

    auto error = 0;
    for (auto &sink : m_sinks)
        error += abs(sink->Message(type, msg));
    return error != 0 ? -1 : 0;
}

void Logger::AddSink(shared_ptr<ILoggerSink> sink)
{
    lock_guard<mutex> lock(m_mutex);

    m_sinks.push_back(sink);
}

void Logger::Error(string msg, ...)
{
    va_list args;
    va_start(args, msg);
    error << StringFormat(msg, args) << endl;
    va_end(args);
}

void Logger::Warning(string msg, ...)
{
    va_list args;
    va_start(args, msg);
    warning << StringFormat(msg, args) << endl;
    va_end(args);
}

void Logger::Trace(string msg, ...)
{
    va_list args;
    va_start(args, msg);
    trace << StringFormat(msg, args) << endl;
    va_end(args);
}
