#pragma once

#include <Audioclient.h>

#include "common/common.h"
#include "common/threading/thread_base.h"
#include "common/logging/logger.h"

#include "media/audio_format.h"
#include "media/interfaces/device_types.h"
#include "media/interfaces/i_device.h"


class WASAPIDevice : public IDevice, protected ThreadBase
{
public:
    explicit WASAPIDevice(DeviceDescriptor& deviceInfo, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIDevice();

    void Start() override;
    void Stop() override;
    void Reset() override;

protected:
    HRESULT InitializeAudioClient(IAudioClient** ppAudioClient, AudioFormat& audioFormat);

protected:
    uint64_t            m_bufferTime;
    DeviceDescriptor    m_descriptor;
    Logger&             m_logger;
};
