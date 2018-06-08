#pragma once

#include <map>

#include "media/interfaces/i_plugin_manager.h"
#include "media/interfaces/i_plugin_provider.h"

class PluginManager : public IPluginManager
{
    typedef std::map<std::string, std::shared_ptr<IPluginProvider>> ProvidersCollection;

public:
    PluginManager();
    virtual ~PluginManager();

    std::vector<std::string> GetGroups() override;
    std::vector<PluginDescriptor> GetGroupPlugins(std::string group) override;
    std::shared_ptr<IProcessor> CreatePlugin(std::string group, const PluginDescriptor& descriptor, const PluginSettings& settings) override;

protected:
    static void InitProvidersCollection(ProvidersCollection& collection);

private:
    ProvidersCollection     m_providers;
};