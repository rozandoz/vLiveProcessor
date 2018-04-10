#pragma once

#include <map>

#include "../interfaces/i_device_manager.h"
#include "../interfaces/i_device_provider.h"

class DeviceManager : public IDeviceManager
{
    typedef std::map<std::string, std::shared_ptr<IDeviceProvider>> ProvidersCollection;

public:
    DeviceManager();
    virtual ~DeviceManager();

    std::vector<std::string> GetGroups() override;
    std::vector<DeviceDescriptor> GetGroupDevices(std::string group, DeviceType type) override;
    std::shared_ptr<IDevice> CreateDevice(std::string group, DeviceType type, DeviceDescriptor& descriptor) override;

protected:
    static void InitProvidersCollection(ProvidersCollection& collection);

private:
    ProvidersCollection     m_providers;
};