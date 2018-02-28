#pragma once

#include "device_manager.h"

class AudioDevice
{
protected:
    AudioDevice(DeviceInfo& deviceInfo)
        : m_deviceInfo(deviceInfo)
    {
    }

    ~AudioDevice()
    {
    }

public:
    virtual HRESULT Start() = 0;
    virtual HRESULT Stop() = 0;
    virtual HRESULT Reset() = 0;

protected:
    DeviceInfo m_deviceInfo;
};
