#include "vst2_processor.h"

#include "media/plugins/vst2/vst2_plugin.h"

using namespace std;

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
    settings.windowController = m_settings.windowController;

    m_plugin = make_shared<VST2Plugin>(settings);
}

void VST2Processor::OnValidateFormat(const AudioFormat& format)
{
    if (format.audioType() != IEEE_FLOAT)
        throw exception("AudioFormat is not supported. Only 'IEEE_FLOAT' formats are supported");
}

bool VST2Processor::OnAddBlock(uint32_t timeout, shared_ptr<MediaBlock> block)
{
    if (m_memoryAllocator == nullptr)
    {
        m_memoryAllocator = MemoryAllocator::Create(block->buffer()->max_size(), 4);
    }

    shared_ptr<Buffer> outputBuffer;
    if (!m_memoryAllocator->TryGetBuffer(timeout, outputBuffer))
        return false;

    auto inputBuffer = block->buffer();

    auto pInData = reinterpret_cast<float*>(inputBuffer->data());
    auto pOutData = reinterpret_cast<float*>(outputBuffer->data());

    auto totalFrames = inputBuffer->size() / m_audioFormat.blockAlign();
    while (totalFrames > 0)
    {
        auto frames = m_plugin->ProcessInterlaved(pInData, pOutData, totalFrames);

        pInData += frames * 2;
        pOutData += frames * 2;

        totalFrames -= frames;
    }

    outputBuffer->set_size(inputBuffer->size());

    return m_consumer->AddBlock(timeout, make_shared<MediaBlock>(outputBuffer, m_audioFormat));
}
