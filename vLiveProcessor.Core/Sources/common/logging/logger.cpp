#include "logger.h"

#include "console_logger_sink.h"

using namespace std;

Logger& Logger::GetInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : error([&](string msg) -> int { return Message(Error, msg); })
    , warning([&](string msg) -> int { return Message(Warning, msg); })
    , trace([&](string msg) -> int { return Message(Trace, msg); })
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
