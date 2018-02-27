#include "strings.h"

#include <cstdarg>
#include <vector>

using namespace std;

string string_format(const string format, ...)
{
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(nullptr, 0, format.c_str(), args);
    va_end(args);
    vector<char> vec(len + 1);
    va_start(args, format);
    vsnprintf(&vec[0], len + 1, format.c_str(), args);
    va_end(args);
    return &vec[0];
}

wstring wstring_format(const wstring format, ...)
{
    va_list args;
    va_start(args, format);
    size_t len = vswprintf(nullptr, 0, format.c_str(), args);
    va_end(args);
    vector<wchar_t> vec(len + 1);
    va_start(args, format);
    vswprintf(&vec[0], len + 1, format.c_str(), args);
    va_end(args);
    return &vec[0];
}
