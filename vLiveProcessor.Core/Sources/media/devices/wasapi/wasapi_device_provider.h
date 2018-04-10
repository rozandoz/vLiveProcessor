#pragma once

#include <mmdeviceapi.h>
#include <Audioclient.h>

#include "../../interfaces/i_device_provider.h"

class WASAPIDeviceProvider : public IDeviceProvider
{
public:
    WASAPIDeviceProvider();
    virtual ~WASAPIDeviceProvider();

private:
    static EDataFlow ToEDataFlow(DeviceType type);

public:
    static HRESULT CreateDeviceEnumerator(IMMDeviceEnumerator** ppEnumerator);
    static HRESULT ActivateAudioDevice(DeviceDescriptor& descriptor, IAudioClient** ppClient);
    static HRESULT GetDevices(EDataFlow dataFlow, DWORD flastateMaskgs, std::vector<DeviceDescriptor>& descriptors);

public: //IDeviceProvider
    std::string Group() override;
    std::vector<DeviceDescriptor> EnumerateDevices(DeviceType type) override;
    std::shared_ptr<IDevice> CreateDevice(DeviceType type, DeviceDescriptor& descriptor) override;
};
