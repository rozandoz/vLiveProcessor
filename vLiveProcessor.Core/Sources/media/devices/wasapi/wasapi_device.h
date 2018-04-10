#pragma once

#include <Audioclient.h>

#include "common.h"
#include "threading/thread_base.h"
#include "../../interfaces/device_types.h"
#include "../../interfaces/i_device.h"

class WASAPIDevice : public IDevice, protected ThreadBase
{
public:
    explicit WASAPIDevice(DeviceDescriptor& deviceInfo, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIDevice();

    void Start() override;
    void Stop() override;
    void Reset() override;

protected:
    HRESULT InitializeAudioClient(IAudioClient** ppAudioClient, WAVEFORMATEX** pWaveFormat);

protected:
    uint64_t            m_bufferTime;
    DeviceDescriptor    m_descriptor;
};
