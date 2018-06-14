#include "strings.h"

#include <cstdarg>
#include <vector>

using namespace std;

string StringFormat(const string format, ...)
{
    va_list args;
    va_start(args, format);
    auto str = StringFormatArgs(format, args);
    va_end(args);
    return str;
}

string StringFormatArgs(const string format, va_list args)
{
    size_t len = vsnprintf(nullptr, 0, format.c_str(), args);
    vector<char> vec(len + 1);
    vsnprintf(&vec[0], len + 1, format.c_str(), args);
    return &vec[0];
}

wstring WStringFormat(const wstring format, ...)
{
    va_list args;
    va_start(args, format);
    auto str = WStringFormatArgs(format, args);
    va_end(args);
    return str;
}

wstring WStringFormatArgs(const wstring format, va_list args)
{
    size_t len = vswprintf(nullptr, 0, format.c_str(), args);
    vector<wchar_t> vec(len + 1);
    vswprintf(&vec[0], len + 1, format.c_str(), args);
    return &vec[0];
}
