#pragma once

#include "../device_manager.h"
#include "../audio_device.h"
#include "../../common/common.h"
#include "../../threading/thread_base.h"

class WASAPIDevice : public AudioDevice, protected ThreadBase
{
public:
    explicit WASAPIDevice(DeviceInfo& deviceInfo, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIDevice();

    HRESULT Start() override;
    HRESULT Stop() override;
    HRESULT Reset() override;

protected:
    HRESULT InitializeAudioClient(IAudioClient** ppAudioClient, WAVEFORMATEX** pWaveFormat);

protected:
    uint64_t m_bufferTime;
};
