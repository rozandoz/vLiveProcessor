#pragma once

#include <vector>
#include <memory>

#include "media/interfaces/i_processor.h"
#include "media/interfaces/plugin_types.h"

class IPluginManager
{
protected:
    virtual ~IPluginManager() {}

public:
    virtual std::vector<std::string> GetGroups() = 0;
    virtual std::vector<PluginDescriptor> GetGroupPlugins(std::string group) = 0;
    virtual std::shared_ptr<IProcessor> CreatePlugin(std::string group, const PluginDescriptor& descriptor, const PluginSettings& settings) = 0;
};