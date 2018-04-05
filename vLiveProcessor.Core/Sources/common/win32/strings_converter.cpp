#include "strings_converter.h"

#include "windows.h"

using namespace std;

string common::win32::WideToMultiByte(wstring wstr)
{
    if (wstr.empty()) return string();

    auto stringSize = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], int(wstr.size()), nullptr, 0, nullptr, nullptr);

    string str(stringSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], int(wstr.size()), &str[0], stringSize, nullptr, nullptr);
    return str;
}

wstring common::win32::MultibyteToWide(string str)
{
    if (str.empty()) return wstring();

    auto stringSize = MultiByteToWideChar(CP_UTF8, 0, &str[0], int(str.size()), nullptr, 0);

    wstring wstr(stringSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], int(str.size()), &wstr[0], stringSize);
    return wstr;
}
