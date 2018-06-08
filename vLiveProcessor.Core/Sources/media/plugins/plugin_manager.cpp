#include "plugin_manager.h"

#include "media/plugins/vst2/vst2_plugin_provider.h"

using namespace std;

PluginManager::PluginManager()
{
    InitProvidersCollection(m_providers);
}

PluginManager::~PluginManager()
{
}

std::vector<std::string> PluginManager::GetGroups()
{
    vector<string> groups;
    for (auto& provider : m_providers)
        groups.push_back(provider.first);
    return groups;
}

std::vector<PluginDescriptor> PluginManager::GetGroupPlugins(std::string group)
{
    if (!m_providers.count(group))
        throw exception("Group is not found");

    return m_providers[group]->EnumeratePlugins();
}

std::shared_ptr<IProcessor> PluginManager::CreatePlugin(std::string group, const PluginDescriptor& descriptor, const PluginSettings& settings)
{
    if (!m_providers.count(group))
        throw exception("Group is not found");

    return m_providers[group]->CreatePlugin(descriptor, settings);
}

void PluginManager::InitProvidersCollection(ProvidersCollection& collection)
{
    shared_ptr<IPluginProvider> providers[] =
    {
        make_shared<VST2PluginProvider>()
    };

    for (auto provider : providers)
    {
        collection[provider->Group()] = provider;
    }
}
