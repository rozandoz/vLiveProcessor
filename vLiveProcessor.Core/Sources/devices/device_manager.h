#pragma once

#include <map>

#include "interfaces/i_device_manager.h"
#include "interfaces/i_device_provider.h"

class DeviceManager : public IDeviceManager
{
    typedef std::map<std::wstring, std::shared_ptr<IDeviceProvider>> ProvidersCollection;

public:
    DeviceManager();
    virtual ~DeviceManager();

    std::vector<std::wstring> GetGroups() override;
    std::vector<DeviceDescriptor> GetGroupDevices(std::wstring group, DeviceType type) override;
    std::shared_ptr<IDevice> CreateDevice(std::wstring group, DeviceType type, DeviceDescriptor& descriptor) override;

protected:
    static void InitProvidersCollection(ProvidersCollection& collection);

private:
    ProvidersCollection     m_providers;
};