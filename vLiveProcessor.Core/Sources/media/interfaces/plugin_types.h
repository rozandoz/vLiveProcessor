#pragma once

#include <string>

struct PluginDescriptor
{
    std::string name;
    std::string location;
};

struct PluginSettings
{
    void *pWindow;
};