#pragma once

#include <memory>

#include "common/memory/memory_allocator.h"

#include "media/interfaces/plugin_types.h"
#include "media/media_processor.h"
#include "media/plugins/vst2/vst2_plugin.h"

class VST2Processor : public MediaProcessor
{
public:
    VST2Processor(const PluginDescriptor& descriptor, const PluginSettings& settings);
    virtual ~VST2Processor();

protected:
    void OnInitialize() override;
    void OnValidateFormat(const AudioFormat& format) override;
    bool OnAddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override;

private:
    PluginDescriptor                    m_descriptor;
    PluginSettings                      m_settings;

    std::shared_ptr<VST2Plugin>         m_plugin;
    std::shared_ptr<MemoryAllocator>    m_memoryAllocator;
};