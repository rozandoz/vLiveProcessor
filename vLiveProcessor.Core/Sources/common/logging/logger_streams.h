#pragma once

#include <sstream>
#include <functional>

typedef std::function<int(std::string)> MessageFunc;

class logger_buf : public std::stringbuf
{
public:
    explicit logger_buf(MessageFunc& func);

protected:
    int sync() override;
    std::streamsize xsputn(const char* ptr, std::streamsize count) override;

private:
    MessageFunc m_func;
};

class logger_stream : public std::ostream
{
public:
    explicit logger_stream(MessageFunc func);

private:
    logger_buf m_buf;
};
