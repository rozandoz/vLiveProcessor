// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <sstream>
#include <iostream>

#include "common/logging/logger.h"
#include "common/win32/hr_exception.h"

class TestClass
{
public:
    TestClass()
    {
        m_a = 123;
    }

    friend std::ostream& operator<<(std::ostream& output, const TestClass& tc)
    {
        output << "TestClass (m_a=" << tc.m_a << ")";
        return output;
    }

private:
    int m_a;
};

class LoggerStrbuffer : public std::stringbuf
{
protected:
    int sync() override
    {
        std::cout << str();
        str(std::string());
        return 0;
    }

    std::streamsize xsputn(const char* ptr, std::streamsize count) override 
    {
        auto size = std::stringbuf::xsputn(ptr, count);
        if (size > 0) this->pubsync();
        return size;
    }
};

int main()
{
    auto& logger = Logger::GetInstance();
       
    logger.warning << "Stream log" << std::endl;
    logger.warning << "Stream log 2" << std::endl;

    TestClass tc;
    logger.warning << tc << std::endl;

    getchar();

    return 0;
}
