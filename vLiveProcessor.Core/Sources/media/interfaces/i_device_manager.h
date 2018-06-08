#pragma once

#include <vector>
#include <memory>

#include "media/interfaces/i_processor.h"
#include "media/interfaces/device_types.h"

class IDeviceManager
{
protected:
    virtual ~IDeviceManager() {}

public:
    virtual std::vector<std::string> GetGroups() = 0;
    virtual std::vector<DeviceDescriptor> GetGroupDevices(std::string group, DeviceType type) = 0;
    virtual std::shared_ptr<IProcessor> CreateDevice(std::string group, DeviceType type, DeviceDescriptor& descriptor) = 0;

};