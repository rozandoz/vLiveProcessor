#include "wasapi_device_provider.h"

#include <memory>

#include <Functiondiscoverykeys_devpkey.h>

#include "win32/hr_exception.h"
#include "wasapi_capture_device.h"
#include "wasapi_render_device.h"

WASAPIDeviceProvider::WASAPIDeviceProvider()
{
    _hr = CoInitialize(nullptr);
}

WASAPIDeviceProvider::~WASAPIDeviceProvider()
{
    CoUninitialize();
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
    try
    {
        if (!ppClient) _hr = E_POINTER;

        CComPtr<IMMDeviceEnumerator> enumerator;
        CComPtr<IMMDevice> device;

        _hr = CreateDeviceEnumerator(&enumerator);
        _hr = enumerator->GetDevice(descriptor.id.c_str(), &device);
        _hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(ppClient));
    }
    catch(hr_exception e)
    {
        return e.Error();
    }
    return S_OK;
}

HRESULT WASAPIDeviceProvider::GetDevices(EDataFlow dataFlow, DWORD flastateMaskgs, std::vector<DeviceDescriptor>& devices)
{
    auto hr = S_OK;
    
    PROPVARIANT prop;
    PropVariantInit(&prop);

    try
    {
        CComPtr<IMMDeviceEnumerator> enumerator;
        CComPtr<IMMDeviceCollection> deviceCollection;
        UINT deviceCount;

        _hr = CreateDeviceEnumerator(&enumerator);
        _hr = enumerator->EnumAudioEndpoints(dataFlow, flastateMaskgs, &deviceCollection);
        _hr = deviceCollection->GetCount(&deviceCount);

        for (UINT i = 0; i < deviceCount; i++)
        {
            DeviceDescriptor descriptor = {};

            //Id

            LPWSTR id;
            CComPtr<IMMDevice> device;
            _hr = deviceCollection->Item(i, &device);
            _hr = device->GetId(&id);

            descriptor.id = id;

            //Friendly name

            CComPtr<IPropertyStore> propertyStore;
            _hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
            _hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &prop);

            descriptor.name = prop.pwszVal;

            devices.push_back(descriptor);
        }

    }
    catch (hr_exception e)
    {
        hr = e.Error();
    }

    PropVariantClear(&prop);
    return hr;
}

std::wstring WASAPIDeviceProvider::Group()
{
    return L"WASAPI";
}

std::vector<DeviceDescriptor> WASAPIDeviceProvider::EnumerateDevices(DeviceType type)
{
    std::vector<DeviceDescriptor> descriptors;
    _hr = GetDevices(ToEDataFlow(type), DEVICE_STATE_ACTIVE, descriptors);
    return descriptors;
}

std::shared_ptr<IDevice> WASAPIDeviceProvider::CreateDevice(DeviceType type, DeviceDescriptor& descriptor)
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
