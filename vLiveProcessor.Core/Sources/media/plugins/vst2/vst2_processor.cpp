#include "vst2_processor.h"

#include "media/plugins/vst2/vst2_plugin.h"

using namespace std;
using namespace chrono;
using namespace common::memory;

VST2Processor::VST2Processor(const PluginDescriptor& descriptor, const PluginSettings& settings)
    : m_descriptor(descriptor)
    , m_settings(settings)
{
}

VST2Processor::~VST2Processor()
{
}

void VST2Processor::OnInitialize()
{
    VST2PluginSettings settings;
    settings.modulePath = m_descriptor.location;
    settings.audioFormat = m_audioFormat;
    settings.blockSize = maxBufferSamples();
    settings.windowController = m_settings.windowController;

    m_plugin = make_shared<VST2Plugin>(settings);
}

void VST2Processor::OnValidateFormat(const AudioFormat& format)
{
    if (format.audioType() != IEEE_FLOAT)
        throw exception("AudioFormat is not supported. Only 'IEEE_FLOAT' formats are supported");
}

bool VST2Processor::OnAddBlock(nanoseconds timeout, shared_ptr<MediaBlock> block)
{
    auto buffer = block->buffer();
    
    auto pData = reinterpret_cast<float*>(buffer->data());
    auto samples = buffer->size() / m_audioFormat.blockAlign();

    while (samples > 0)
    {
        auto frames = m_plugin->ProcessInterlaved(pData, pData, samples); // this method makes double copying to and from internal buffer

        pData += frames * m_audioFormat.channels();
        samples -= frames;
    }

    return m_consumer->AddBlock(timeout, make_shared<MediaBlock>(buffer, m_audioFormat));
}
