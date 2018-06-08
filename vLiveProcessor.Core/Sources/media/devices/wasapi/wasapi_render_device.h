#pragma once
#include "wasapi_device.h"

#include "common/memory/memory_allocator.h"

#include "media/interfaces/i_consumer.h"

class WASAPIRenderDevice : public WASAPIDevice
{
public:
    explicit WASAPIRenderDevice(DeviceDescriptor& descriptor, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPIRenderDevice();

protected:
    bool OnAddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override;
    void OnValidateFormat(const AudioFormat& format) override;

private:
    void OnThreadProc() override;

private:
    std::mutex                              m_critSec;
    std::queue<std::shared_ptr<Buffer>>     m_queue;
};
