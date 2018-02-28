#include "device_manager.h"

#include <atlcomcli.h>
#include <Functiondiscoverykeys_devpkey.h>

HRESULT CreateDeviceEnumerator(IMMDeviceEnumerator** ppEnumerator)
{
    return CoCreateInstance(__uuidof(MMDeviceEnumerator),
                            nullptr,
                            CLSCTX_ALL,
                            __uuidof(IMMDeviceEnumerator),
                            reinterpret_cast<void**>(ppEnumerator));
}

HRESULT ActivateAudioDevice(DeviceInfo& deviceInfo, IAudioClient** ppClient)
{
    auto hr = S_OK;

    CComPtr<IMMDeviceEnumerator> enumerator;
    CComPtr<IMMDevice> device;

    if (SUCCEEDED(hr)) hr = CreateDeviceEnumerator(&enumerator);
    if (SUCCEEDED(hr)) hr = enumerator->GetDevice(deviceInfo.id.c_str(), &device);
    if (SUCCEEDED(hr)) hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(ppClient));

    return hr;
}

HRESULT GetDevices(EDataFlow dataFlow, DWORD flastateMaskgs, std::vector<DeviceInfo>& devices)
{
    auto hr = S_OK;

    CComPtr<IMMDeviceEnumerator> enumerator;
    CComPtr<IMMDeviceCollection> deviceCollection;
    UINT deviceCount;

    if (SUCCEEDED(hr)) hr = CreateDeviceEnumerator(&enumerator);
    if (SUCCEEDED(hr)) hr = enumerator->EnumAudioEndpoints(dataFlow, flastateMaskgs, &deviceCollection);
    if (SUCCEEDED(hr)) hr = deviceCollection->GetCount(&deviceCount);
    if (FAILED(hr)) return hr;

    PROPVARIANT prop;
    PropVariantInit(&prop);

    for (UINT i = 0; i < deviceCount; i++)
    {
        DeviceInfo info = {};

        //Id

        LPWSTR id;
        CComPtr<IMMDevice> device;
        if (SUCCEEDED(hr)) hr = deviceCollection->Item(i, &device);
        if (SUCCEEDED(hr)) hr = device->GetId(&id);
        if (FAILED(hr)) return hr;

        info.id = id;

        //Friendly name

        CComPtr<IPropertyStore> propertyStore;
        if (SUCCEEDED(hr)) hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
        if (SUCCEEDED(hr)) hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &prop);
        if (FAILED(hr)) return hr;

        info.name = prop.pwszVal;

        devices.push_back(info);
    }

    PropVariantClear(&prop);

    return hr;
}
