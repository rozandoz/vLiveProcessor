#pragma once
#include "wasapi_device.h"

#include "memory/memory_allocator.h"
#include "../media/interfaces/i_producer.h"

#define BUFFERS_COUNT 4

class WASAPICaptureDevice : public WASAPIDevice, public IProducer
{
public:
    explicit WASAPICaptureDevice(DeviceDescriptor& descriptor, uint64_t bufferTime = REFTIMES_PER_SEC);
    virtual ~WASAPICaptureDevice();

    bool TryGetBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override;

private:
    void OnThreadProc() override;

private:
    std::mutex                              m_critSec;
    std::queue<std::shared_ptr<Buffer>>     m_queue;
};
