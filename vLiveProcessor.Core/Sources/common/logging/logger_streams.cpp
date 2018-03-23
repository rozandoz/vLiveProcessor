#include "logger_streams.h"

using namespace std;

logger_buf::logger_buf(MessageFunc& func) 
    : m_func(func)
{
}

int logger_buf::sync()
{
    auto r = m_func(str());
    str(string());
    return r;
}

streamsize logger_buf::xsputn(const char* ptr, streamsize count)
{
    auto size = stringbuf::xsputn(ptr, count);
    if (size > 0) this->pubsync();
    return size;
}

logger_stream::logger_stream(MessageFunc func)
    : basic_ostream<char, char_traits<char>>(nullptr)
    , m_buf(func)
{
    this->init(&m_buf);
}
