#include "wasapi_device_provider.h"

#include <memory>

#include <atlcomcli.h>
#include <Functiondiscoverykeys_devpkey.h>

#include "win32/hr_exception.h"
#include "wasapi_capture_device.h"
#include "wasapi_render_device.h"

WASAPIDeviceProvider::WASAPIDeviceProvider()
{
}

WASAPIDeviceProvider::~WASAPIDeviceProvider()
{
}

EDataFlow WASAPIDeviceProvider::ToEDataFlow(DeviceType type)
{
    if (type == All) return eAll;
    if (type == Capture) return eCapture;
    if (type == Render) return eRender;

    throw std::runtime_error("Device type is not supported");
}

HRESULT WASAPIDeviceProvider::CreateDeviceEnumerator(IMMDeviceEnumerator** ppEnumerator)
{
    return CoCreateInstance(__uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(ppEnumerator));
}

HRESULT WASAPIDeviceProvider::ActivateAudioDevice(DeviceDescriptor& descriptor, IAudioClient** ppClient)
{
    auto hr = S_OK;

    CComPtr<IMMDeviceEnumerator> enumerator;
    CComPtr<IMMDevice> device;

    if (SUCCEEDED(hr)) hr = CreateDeviceEnumerator(&enumerator);
    if (SUCCEEDED(hr)) hr = enumerator->GetDevice(descriptor.id.c_str(), &device);
    if (SUCCEEDED(hr)) hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(ppClient));

    return hr;
}

HRESULT WASAPIDeviceProvider::GetDevices(EDataFlow dataFlow, DWORD flastateMaskgs, std::vector<DeviceDescriptor>& devices)
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
        DeviceDescriptor descriptor = {};

        //Id

        LPWSTR id;
        CComPtr<IMMDevice> device;
        if (SUCCEEDED(hr)) hr = deviceCollection->Item(i, &device);
        if (SUCCEEDED(hr)) hr = device->GetId(&id);
        if (FAILED(hr)) return hr;

        descriptor.id = id;

        //Friendly name

        CComPtr<IPropertyStore> propertyStore;
        if (SUCCEEDED(hr)) hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
        if (SUCCEEDED(hr)) hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &prop);
        if (FAILED(hr)) return hr;

        descriptor.name = prop.pwszVal;

        devices.push_back(descriptor);
    }

    PropVariantClear(&prop);

    return hr;
}

std::wstring WASAPIDeviceProvider::Group()
{
    return L"WASAPI";
}

std::vector<DeviceDescriptor> WASAPIDeviceProvider::EnumerateDevices(DeviceType& type)
{
    std::vector<DeviceDescriptor> descriptors;
    _hr = GetDevices(ToEDataFlow(type), 0, descriptors);
    return descriptors;
}

std::shared_ptr<IDevice> WASAPIDeviceProvider::CreateDevice(DeviceType& type, DeviceDescriptor& descriptor)
{
    switch (type)
    {
    case Capture:
        return std::make_shared<WASAPICaptureDevice>(descriptor);
    case Render:
        return std::make_shared<WASAPIRenderDevice>(descriptor);
    default:
        break;
    }

    throw new std::runtime_error("Invalid device type");
}
