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

vector<wstring> DeviceManager::GetGroups()
{
    vector<wstring> groups;
    for (auto& provider : m_providers)
        groups.push_back(provider.first);
    return groups;
}

vector<DeviceDescriptor> DeviceManager::GetGroupDevices(wstring group, DeviceType type)
{
    if (!m_providers.count(group))
        throw new exception("Group is not found");

    return m_providers[group]->EnumerateDevices(type);
}

shared_ptr<IDevice> DeviceManager::CreateDevice(wstring group, DeviceType type, DeviceDescriptor& descriptor)
{
    if (!m_providers.count(group))
        throw new exception("Group is not found");

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
