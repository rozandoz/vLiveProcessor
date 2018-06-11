#pragma once

#include <string>
#include <memory>

#include "i_plugin_window_controller.h"

struct PluginDescriptor
{
    std::string name;
    std::string location;
};

struct PluginSettings
{
    std::shared_ptr<IPluginWindowController> windowController;
};