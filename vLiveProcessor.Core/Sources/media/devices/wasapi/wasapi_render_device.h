#pragma once

#include "wasapi_device.h"
#include "memory/memory_allocator.h"

class WASAPIRenderDevice : public WASAPIDevice
{
public:
    explicit WASAPIRenderDevice(DeviceDescriptor& descriptor, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIRenderDevice();

    HRESULT RenderBuffer(std::shared_ptr<Buffer> buffer);
private:
    void OnThreadProc() override;

private:
    std::mutex                              m_critSec;
    std::queue<std::shared_ptr<Buffer>>     m_queue;
};
