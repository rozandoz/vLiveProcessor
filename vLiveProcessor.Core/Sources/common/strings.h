#pragma once

#include <string>

std::string StringFormat(const std::string format, ...);
std::string StringFormatArgs(const std::string format, va_list list);

std::wstring WStringFormat(const std::wstring format, ...);
std::wstring WStringFormatArgs(const std::wstring format, va_list list);
