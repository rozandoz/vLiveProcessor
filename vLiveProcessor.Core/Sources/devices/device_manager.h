#pragma once

#include "../interfaces/i_device_provider.h"

#include <vector>

#include <mmdeviceapi.h>
#include <Audioclient.h>


HRESULT CreateDeviceEnumerator(IMMDeviceEnumerator** ppEnumerator);
HRESULT ActivateAudioDevice(DeviceDescriptor& deviceInfo, IAudioClient** ppClient);
HRESULT GetDevices(EDataFlow dataFlow, DWORD flastateMaskgs, std::vector<DeviceDescriptor>& devices);
