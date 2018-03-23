#pragma once

#include <memory>
#include <vector>

#include "i_device.h"
#include "device_types.h"

class IDeviceProvider
{
protected:
    IDeviceProvider()
    {
    }

    virtual ~IDeviceProvider()
    {
    }

public:
    virtual std::wstring Group() = 0;
    virtual std::vector<DeviceDescriptor> EnumerateDevices(DeviceType& type) = 0;
    virtual std::shared_ptr<IDevice> CreateDevice(DeviceType& type, DeviceDescriptor& descriptor) = 0;
};