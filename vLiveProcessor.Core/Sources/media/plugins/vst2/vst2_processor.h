#pragma once

#include <memory>

#include "common/threading/queue.h"
#include "common/memory/memory_allocator.h"

#include "media/interfaces/plugin_types.h"
#include "media/media_processor_async.h"
#include "media/plugins/vst2/vst2_plugin.h"

class VST2Processor : public MediaProcessorAsync
{
public:
    VST2Processor(const PluginDescriptor& descriptor, const PluginSettings& settings);
    virtual ~VST2Processor();

protected:
    void OnInitialize() override;
    void OnValidateFormat(const AudioFormat& format) override;
    bool OnAddBlock(std::chrono::nanoseconds timeout, std::shared_ptr<MediaBlock> block) override;
    void OnThreadProc() override;

private:
    PluginDescriptor                                                        m_descriptor;
    PluginSettings                                                          m_settings;

    std::shared_ptr<VST2Plugin>                                             m_plugin;
    std::shared_ptr<common::memory::MemoryAllocator>                        m_memoryAllocator;
    common::threading::Queue<std::shared_ptr<common::memory::Buffer>>       m_queue;
};