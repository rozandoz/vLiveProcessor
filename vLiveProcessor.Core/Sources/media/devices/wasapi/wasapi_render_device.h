#pragma once
#include "wasapi_device.h"

#include "common/memory/memory_allocator.h"

#include "../../interfaces/i_consumer.h"

class WASAPIRenderDevice : public WASAPIDevice, public IConsumer
{
public:
    explicit WASAPIRenderDevice(DeviceDescriptor& descriptor, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIRenderDevice();

    bool TryPushBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override;

private:
    void OnThreadProc() override;

private:
    std::mutex                              m_critSec;
    std::queue<std::shared_ptr<Buffer>>     m_queue;
};
