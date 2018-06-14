#pragma once

#include <Audioclient.h>

#include "media/media_processor_async.h"
#include "media/interfaces/device_types.h"

class WASAPIDevice : public MediaProcessorAsync
{
public:
    explicit WASAPIDevice(DeviceDescriptor& deviceInfo);
    virtual ~WASAPIDevice();

protected:
    static AudioFormat ToAudioFormat(const WAVEFORMATEX* pWaveFormat);
    static WAVEFORMATEX* FromAudioFormat(const AudioFormat& audioFormat);

    HRESULT InitializeAudioClient(IAudioClient** ppAudioClient, AudioFormat& audioFormat);

protected:
    DeviceDescriptor    m_descriptor;
};
