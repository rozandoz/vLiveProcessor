#include "vst2_processor.h"

#include "media/plugins/vst2/vst2_plugin.h"

using namespace std;
using namespace chrono;

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
    m_memoryAllocator = MemoryAllocator::Create(bufferSamples() * m_audioFormat.blockAlign(), buffersCount());
    m_queue.SetCapacity(buffersCount());

    VST2PluginSettings settings;
    settings.modulePath = m_descriptor.location;
    settings.audioFormat = m_audioFormat;
    settings.blockSize = bufferSamples(); // for one buffer
    settings.windowController = m_settings.windowController;

    m_plugin = make_shared<VST2Plugin>(settings);
}

void VST2Processor::OnValidateFormat(const AudioFormat& format)
{
    if (format.audioType() != IEEE_FLOAT)
        throw exception("AudioFormat is not supported. Only 'IEEE_FLOAT' formats are supported");
}

bool VST2Processor::OnAddBlock(milliseconds timeout, shared_ptr<MediaBlock> block)
{
    return m_queue.TryAdd(timeout, block->buffer());
}

void VST2Processor::OnThreadProc()
{
    auto waitTime = AudioFormat::GetDurationMs(m_audioFormat, bufferSamples());

    while (!CheckClosing())
    {
        shared_ptr<Buffer> inputBuffer;
        if (!m_queue.TryGet(waitTime, inputBuffer))
            continue;

        shared_ptr<Buffer> outputBuffer;
        if (m_memoryAllocator->TryGetBuffer(waitTime, outputBuffer))
        {
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
            inputBuffer.reset();


            if (!m_consumer->AddBlock(waitTime, make_shared<MediaBlock>(outputBuffer, m_audioFormat)))
                m_logger.warning << "VST2Processor: failed to push block to consumer";
        }
    }
}
