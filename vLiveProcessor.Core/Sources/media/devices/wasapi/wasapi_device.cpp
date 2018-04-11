#include "wasapi_device.h"

#include "common/win32/hr_exception.h"

#include "wasapi_device_provider.h"

using namespace common::win32;

WASAPIDevice::WASAPIDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : m_bufferTime(bufferTime)
    , m_descriptor(descriptor)
    , m_logger(Logger::GetInstance())
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

HRESULT WASAPIDevice::InitializeAudioClient(IAudioClient** ppAudioClient, AudioFormat& audioFormat)
{
    auto hr = S_OK;

    WAVEFORMATEX *pWaveFormat;

    try
    {
        if (!ppAudioClient) _hr = E_POINTER;

        CComPtr<IAudioClient> audioClient;
        _hr = WASAPIDeviceProvider::ActivateAudioDevice(m_descriptor, &audioClient);

        _hr = audioClient->GetMixFormat(&pWaveFormat);
        _hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, m_bufferTime, 0, pWaveFormat, nullptr);
        _hr = audioClient.QueryInterface(ppAudioClient);

        audioFormat = AudioFormat(pWaveFormat->nChannels, pWaveFormat->wBitsPerSample, pWaveFormat->nSamplesPerSec);
    }
    catch (hr_exception e)
    {
        hr = e.Error();
    }

    if (pWaveFormat)
        CoTaskMemFree(pWaveFormat);

    return hr;
}