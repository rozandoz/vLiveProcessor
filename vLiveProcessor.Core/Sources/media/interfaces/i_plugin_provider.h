#pragma once

#include <memory>
#include <vector>

#include "media/interfaces/i_processor.h"
#include "media/interfaces/plugin_types.h"

class IPluginProvider
{
protected:
    virtual ~IPluginProvider() {}

public:
    virtual std::string Group() = 0;
    virtual std::vector<PluginDescriptor> EnumeratePlugins() = 0;
    virtual std::shared_ptr<IProcessor> CreatePlugin(const PluginDescriptor& descriptor, const PluginSettings& settings) = 0;
};
