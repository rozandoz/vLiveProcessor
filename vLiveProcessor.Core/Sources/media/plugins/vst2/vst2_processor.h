#pragma once

#include <memory>

#include "media/interfaces/plugin_types.h"
#include "media/media_processor_async.h"
#include "media/plugins/vst2/vst2_plugin.h"

class VST2Processor : public MediaProcessor
{
public:
    VST2Processor(const PluginDescriptor& descriptor, const PluginSettings& settings);
    virtual ~VST2Processor();

protected:
    void OnInitialize() override;
    void OnValidateFormat(const AudioFormat& format) override;
    bool OnAddBlock(std::chrono::nanoseconds timeout, std::shared_ptr<MediaBlock> block) override;

private:
    PluginDescriptor                                                        m_descriptor;
    PluginSettings                                                          m_settings;

    std::shared_ptr<VST2Plugin>                                             m_plugin;
};