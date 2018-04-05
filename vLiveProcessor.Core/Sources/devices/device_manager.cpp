#include "device_manager.h"

#include "strings.h"
#include "wasapi/wasapi_device_provider.h"

using namespace std;

DeviceManager::DeviceManager()
{
    InitProvidersCollection(m_providers);
}

DeviceManager::~DeviceManager()
{
}

vector<string> DeviceManager::GetGroups()
{
    vector<string> groups;
    for (auto& provider : m_providers)
        groups.push_back(provider.first);
    return groups;
}

vector<DeviceDescriptor> DeviceManager::GetGroupDevices(string group, DeviceType type)
{
    if (!m_providers.count(group))
        throw exception("Group is not found");

    return m_providers[group]->EnumerateDevices(type);
}

shared_ptr<IDevice> DeviceManager::CreateDevice(string group, DeviceType type, DeviceDescriptor& descriptor)
{
    if (!m_providers.count(group))
        throw exception("Group is not found");

    return m_providers[group]->CreateDevice(type, descriptor);
}

void DeviceManager::InitProvidersCollection(ProvidersCollection& collection)
{
    shared_ptr<IDeviceProvider> providers[] =
    {
        make_shared<WASAPIDeviceProvider>()
    };

    for (auto provider : providers)
    {
        collection[provider->Group()] = provider;
    }
}
