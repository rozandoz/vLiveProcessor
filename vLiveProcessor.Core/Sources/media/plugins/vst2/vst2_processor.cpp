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
    settings.sampleRate = m_audioFormat.samplesPerSec();
    settings.windowController = m_settings.windowController;

    m_plugin = make_shared<VST2Plugin>(settings);
}

void VST2Processor::OnValidateFormat(const AudioFormat& format)
{
    if (format.audioType() != IEEE_FLOAT)
        throw exception("AudioFormat is not supported. Only 'IEEE_FLOAT' formats are supported");
}

bool VST2Processor::OnAddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block)
{
    if (m_memoryAllocator == nullptr)
    {
        m_memoryAllocator = MemoryAllocator::Create(block->buffer()->max_size(), 20);
    }

    std::shared_ptr<Buffer> outputBuffer;
    m_memoryAllocator->TryGetBuffer(100, outputBuffer);

    auto audioFormat = block->audioFormat();
    auto inputBuffer = block->buffer();

    auto pInData = reinterpret_cast<float*>(inputBuffer->data());
    auto pOutData = reinterpret_cast<float*>(outputBuffer->data());

    auto totalFrames = inputBuffer->size() / audioFormat.blockAlign();
    while (totalFrames > 0)
    {
        auto frames = totalFrames;
        auto result = m_plugin->Process(pInData, frames);

        for (size_t i = 0; i < frames; i++)
        {
            auto offset = i * 2;
            memcpy(pOutData + offset, result[0] + i, sizeof(float));
            memcpy(pOutData + offset + 1, result[1] + i, sizeof(float));
        }

        pInData += frames * 2;
        pOutData += frames * 2;

        totalFrames -= frames;
    }

    outputBuffer->set_size(inputBuffer->size());

    return m_consumer->AddBlock(timeout, std::make_shared<MediaBlock>(outputBuffer, audioFormat));
}
