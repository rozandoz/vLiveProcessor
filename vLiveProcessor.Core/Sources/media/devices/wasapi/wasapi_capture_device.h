#pragma once
#include "wasapi_device.h"

#include <atlcomcli.h>

class WASAPICaptureDevice : public WASAPIDevice
{
public:
    explicit WASAPICaptureDevice(DeviceDescriptor& descriptor, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPICaptureDevice();

protected:
    void OnInitialize() override;

private:
    void OnThreadProc() override;

private:
    std::mutex                                              m_critSec;
    
    CComPtr<IAudioClient>                                   m_audioClient;
};
