#include "wasapi_device.h"

#include "common/win32/hr_exception.h"

#include "wasapi_device_provider.h"

using namespace common::win32;

WASAPIDevice::WASAPIDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : m_bufferTime(bufferTime)
    , m_descriptor(descriptor)
{
}

WASAPIDevice::~WASAPIDevice()
{
}

AudioFormat WASAPIDevice::ToAudioFormat(const WAVEFORMATEX* pWaveFormat)
{
    auto type = INVALID;

    if (pWaveFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE && pWaveFormat->cbSize == sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))
    {
        auto pExtensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(pWaveFormat);

        if (pExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) type = IEEE_FLOAT;
        if (pExtensible->SubFormat == KSDATAFORMAT_SUBTYPE_PCM) type = PCM;
    }
    else
    {
        if (pWaveFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)  type = IEEE_FLOAT;
        if (pWaveFormat->wFormatTag == WAVE_FORMAT_PCM)  type = PCM;
    }

    if (type == INVALID) throw std::exception("Audio subtype is not supported");

    return AudioFormat(type, pWaveFormat->nChannels, pWaveFormat->wBitsPerSample, pWaveFormat->nSamplesPerSec);
}

WAVEFORMATEX* WASAPIDevice::FromAudioFormat(const AudioFormat& audioFormat)
{
    auto subFormat = GUID_NULL;

    switch (audioFormat.audioType())
    {
    case PCM: subFormat = KSDATAFORMAT_SUBTYPE_PCM; break;
    case IEEE_FLOAT: subFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT; break;
    default: break;
    }

    if (subFormat == GUID_NULL) throw std::exception("Audio subtype is not supported");

    auto pWaveFormat = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(CoTaskMemAlloc(sizeof(WAVEFORMATEXTENSIBLE)));

    pWaveFormat->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    pWaveFormat->Format.nChannels = audioFormat.channels();
    pWaveFormat->Format.nSamplesPerSec = audioFormat.samplesPerSec();
    pWaveFormat->Format.nAvgBytesPerSec = audioFormat.avgBytesPerSec();
    pWaveFormat->Format.nBlockAlign = audioFormat.blockAlign();
    pWaveFormat->Format.wBitsPerSample = audioFormat.bitsPerSample();
    pWaveFormat->Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(pWaveFormat->Format);

    pWaveFormat->Samples.wValidBitsPerSample = audioFormat.bitsPerSample();

    pWaveFormat->dwChannelMask = 0x03; //stereo only
    pWaveFormat->SubFormat = subFormat;

    return reinterpret_cast<WAVEFORMATEX*>(pWaveFormat);
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

        if (audioFormat == INVALID_AUDIO_FORMAT)
            _hr = audioClient->GetMixFormat(&pWaveFormat);
        else
            pWaveFormat = FromAudioFormat(audioFormat);

        _hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, m_bufferTime, 0, pWaveFormat, nullptr);
        _hr = audioClient.QueryInterface(ppAudioClient);

        audioFormat = ToAudioFormat(pWaveFormat);
    }
    catch (hr_exception e)
    {
        hr = e.Error();
    }

    if (pWaveFormat)
        CoTaskMemFree(pWaveFormat);

    return hr;
}
