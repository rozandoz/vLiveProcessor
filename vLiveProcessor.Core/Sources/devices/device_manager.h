#pragma once

#include <vector>

#include <mmdeviceapi.h>
#include <Audioclient.h>

struct DeviceInfo
{
    std::wstring id;
    std::wstring name;
};

HRESULT CreateDeviceEnumerator(IMMDeviceEnumerator** ppEnumerator);
HRESULT ActivateAudioDevice(DeviceInfo& deviceInfo, IAudioClient** ppClient);
HRESULT GetDevices(EDataFlow dataFlow, DWORD flastateMaskgs, std::vector<DeviceInfo>& devices);
