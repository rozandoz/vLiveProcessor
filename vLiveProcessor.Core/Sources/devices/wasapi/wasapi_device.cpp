#include "wasapi_device.h"

#include "../../common/errors.h"

WASAPIDevice::WASAPIDevice(DeviceInfo& deviceInfo, uint64_t bufferTime)
    : AudioDevice(deviceInfo)
    , m_bufferTime(bufferTime)
{
}

WASAPIDevice::~WASAPIDevice()
{
}

HRESULT WASAPIDevice::Start()
{
    return StartThread();
}

HRESULT WASAPIDevice::Stop()
{
    return StopThread();
}

HRESULT WASAPIDevice::Reset()
{
    try
    {
        _hr = Stop();
        _hr = Start();
    }
    catch (hr_exception e)
    {
        return e.Error();
    }

    return S_OK;
}

HRESULT WASAPIDevice::InitializeAudioClient(IAudioClient** ppAudioClient, WAVEFORMATEX** pWaveFormat)
{
    try
    {
        if (!ppAudioClient) _hr = E_POINTER;
        if (!pWaveFormat) _hr = E_POINTER;

        CComPtr<IAudioClient> audioClient;
        _hr = ActivateAudioDevice(m_deviceInfo, &audioClient);
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
