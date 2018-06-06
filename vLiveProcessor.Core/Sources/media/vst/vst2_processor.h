#pragma once

#include <memory>

#include "common/memory/memory_allocator.h"

#include "media/interfaces/i_processor.h"
#include "media/interfaces/i_consumer.h"
#include "media/interfaces/i_producer.h"

#include "media/vst/vst2_host.h"

class VST2Processor : public IConsumer, public IProducer, public IProcessor
{
public:
    VST2Processor(VST2PluginSettings& settings);
    virtual ~VST2Processor();

public:
    bool TryPushBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override;
    void SetConsumer(const std::shared_ptr<IConsumer>& consumer) override;

private:
    std::shared_ptr<VST2Plugin>         m_plugin;
    std::shared_ptr<IConsumer>          m_consumer;

    std::shared_ptr<MemoryAllocator>    m_memoryAllocator;
};