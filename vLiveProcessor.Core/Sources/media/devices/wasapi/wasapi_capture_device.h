#pragma once

#include "wasapi_device.h"
#include "memory/memory_allocator.h"

#define BUFFERS_COUNT 4

class WASAPICaptureDevice : public WASAPIDevice
{
public:
    explicit WASAPICaptureDevice(DeviceDescriptor& descriptor, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPICaptureDevice();

    HRESULT CaptureBuffer(std::shared_ptr<Buffer>& buffer);

private:
    void OnThreadProc() override;

private:
    std::mutex                              m_critSec;
    std::queue<std::shared_ptr<Buffer>>     m_queue;
};
