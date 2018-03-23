#include "logger.h"

#include "console_logger_sink.h"

using namespace std;

Logger::Logger(bool threadSafe)
    : error([&](string msg) -> int { return Message(Error, msg); })
    , warning([&](string msg) -> int { return Message(Warning, msg); })
    , trace([&](string msg) -> int { return Message(Trace, msg); })
    , m_threadSafe(threadSafe)
{
    if (m_threadSafe) m_guard = [&](void) -> void { lock_guard<mutex> lock(m_mutex); };
    else m_guard = [](void)-> void {};

    AddSink(make_shared<ConsoleLoggerSink>());
}

int Logger::Message(MessageType type, std::string msg)
{
    m_guard();

    auto error = 0;
    for (auto &sink : m_sinks)
        error += sink->Message(type, msg);
    return error != 0 ? -1 : 0;
}

void Logger::AddSink(shared_ptr<ILoggerSink> sink)
{
    m_guard();

    m_sinks.push_back(sink);
}
