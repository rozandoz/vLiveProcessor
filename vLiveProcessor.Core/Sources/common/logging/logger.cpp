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
    va_list myargs; 
    va_start(myargs, msg);
    error << StringFormatArgs(msg, myargs) << endl;
    va_end(myargs);
}

void Logger::Warning(string msg, ...)
{
    va_list myargs;
    va_start(myargs, msg);
    warning << StringFormatArgs(msg, myargs) << endl;
    va_end(myargs);
}

void Logger::Trace(string msg, ...)
{
    va_list myargs;
    va_start(myargs, msg);
    trace << StringFormatArgs(msg, myargs) << endl;
    va_end(myargs);
}
