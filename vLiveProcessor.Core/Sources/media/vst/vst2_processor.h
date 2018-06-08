#pragma once

#include <memory>

#include "common/memory/memory_allocator.h"

#include "media/media_processor.h"
#include "media/vst/vst2_host.h"

class VST2Processor : public MediaProcessor
{
public:
    VST2Processor(const VST2PluginSettings& settings);
    virtual ~VST2Processor();

protected:
    void OnInitialize() override;
    void OnValidateFormat(const AudioFormat& format) override;
    bool OnAddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override;

private:
    VST2PluginSettings                  m_settings;

    std::shared_ptr<VST2Plugin>         m_plugin;
    std::shared_ptr<MemoryAllocator>    m_memoryAllocator;
};