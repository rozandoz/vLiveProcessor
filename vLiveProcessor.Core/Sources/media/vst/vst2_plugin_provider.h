#pragma once

#include "media/interfaces/i_plugin_provider.h"

class VST2PluginProvider : public IPluginProvider
{
public:
    VST2PluginProvider();
    virtual ~VST2PluginProvider();

    std::string Group() override;
    std::vector<PluginDescriptor> EnumeratePlugins() override;
    std::shared_ptr<IProcessor> CreatePlugin(const PluginDescriptor& descriptor, const PluginSettings& settings) override;
};
