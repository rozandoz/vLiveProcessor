#include "wasapi_device.h"

#include "win32/hr_exception.h"
#include "wasapi_device_provider.h"

WASAPIDevice::WASAPIDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : m_bufferTime(bufferTime)
    , m_descriptor(descriptor)
{
}

WASAPIDevice::~WASAPIDevice()
{
}

void WASAPIDevice::Start()
{
    StartThread();
}

void WASAPIDevice::Stop()
{
    StopThread();
}

void WASAPIDevice::Reset()
{
    Stop();
    Start();
}

HRESULT WASAPIDevice::InitializeAudioClient(IAudioClient** ppAudioClient, WAVEFORMATEX** pWaveFormat)
{
    try
    {
        if (!ppAudioClient) _hr = E_POINTER;
        if (!pWaveFormat) _hr = E_POINTER;

        CComPtr<IAudioClient> audioClient;
        _hr = WASAPIDeviceProvider::ActivateAudioDevice(m_descriptor, &audioClient);
        _hr = audioClient->GetMixFormat(pWaveFormat);
        _hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, m_bufferTime, 0, *pWaveFormat, nullptr);
        _hr = audioClient.QueryInterface(ppAudioClient);
    }
    catch (hr_exception e)
    {
        return e.Error();
    }

    return S_OK;
}
