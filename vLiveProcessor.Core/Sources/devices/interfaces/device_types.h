#pragma once

#include <string>

enum DeviceType
{
    Capture = 0x01,
    Render = Capture << 1,
    All = Capture | Render
};

struct DeviceDescriptor
{
    std::wstring name;
    std::wstring id;
};