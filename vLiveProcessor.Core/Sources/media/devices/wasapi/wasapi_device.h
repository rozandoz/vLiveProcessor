#pragma once

#include <Audioclient.h>

#include "common/common.h"
#include "media/media_processor_async.h"
#include "media/interfaces/device_types.h"

class WASAPIDevice : public MediaProcessorAsync
{
public:
    explicit WASAPIDevice(DeviceDescriptor& deviceInfo, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIDevice();

protected:
    static AudioFormat ToAudioFormat(const WAVEFORMATEX* pWaveFormat);

    HRESULT InitializeAudioClient(IAudioClient** ppAudioClient, AudioFormat& audioFormat);

protected:
    uint64_t            m_bufferTime;
    DeviceDescriptor    m_descriptor;
};
