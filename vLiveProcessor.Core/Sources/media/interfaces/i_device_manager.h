#pragma once

#include <vector>
#include <memory>

#include "device_types.h"
#include "i_device.h"

class IDeviceManager
{
protected:
    IDeviceManager() {}
    virtual ~IDeviceManager() {}

public:
    virtual std::vector<std::string> GetGroups() = 0;
    virtual std::vector<DeviceDescriptor> GetGroupDevices(std::string group, DeviceType type) = 0;
    virtual std::shared_ptr<IDevice> CreateDevice(std::string group, DeviceType type, DeviceDescriptor& descriptor) = 0;

};