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
    std::string name;
    std::string id;
};