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
    m_memoryAllocator = RingBuffer::Create(maxBufferSamples() * m_audioFormat.blockAlign());

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
    return m_queue.TryAdd(timeout, block->buffer());
}

void VST2Processor::OnThreadProc()
{
    while (!CheckClosing())
    {
        shared_ptr<Buffer> inputBuffer;
        if (!m_queue.TryGet(1ms, inputBuffer))
            continue;

        auto inputSamples = inputBuffer->size() / m_audioFormat.blockAlign();
        auto waitTime = AudioFormat::GetDurationNs(m_audioFormat, inputSamples);

        shared_ptr<Buffer> outputBuffer;
        if (m_memoryAllocator->TryGetBuffer(waitTime, inputBuffer->size(), outputBuffer))
        {
            auto pInData = reinterpret_cast<float*>(inputBuffer->data());
            auto pOutData = reinterpret_cast<float*>(outputBuffer->data());

            auto totalFrames = inputSamples;

            while (totalFrames > 0)
            {
                auto frames = m_plugin->ProcessInterlaved(pInData, pOutData, totalFrames);

                pInData += frames * m_audioFormat.channels();
                pOutData += frames * m_audioFormat.channels();

                totalFrames -= frames;
            }

            outputBuffer->set_size(inputBuffer->size());

            if (!m_consumer->AddBlock(waitTime, make_shared<MediaBlock>(outputBuffer, m_audioFormat)))
                m_logger.warning << "VST2Processor: failed to push block to consumer";
        }
    }
}
