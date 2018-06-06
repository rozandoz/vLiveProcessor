#include "vst2_processor.h"

#include "media/vst/vst2_plugin.h"

VST2Processor::VST2Processor(VST2PluginSettings& settings)
{
    m_plugin = VST2Host::Create()->CreatePlugin(settings);
}

VST2Processor::~VST2Processor()
{
}

bool VST2Processor::TryPushBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block)
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
    while(totalFrames > 0)
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

    return m_consumer->TryPushBlock(timeout, std::make_shared<MediaBlock>(outputBuffer, audioFormat));
}

void VST2Processor::SetConsumer(const std::shared_ptr<IConsumer>& consumer)
{
    m_consumer = consumer;
}
