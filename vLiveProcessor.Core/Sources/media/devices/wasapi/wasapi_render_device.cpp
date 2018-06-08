#include "wasapi_render_device.h"

#include <iostream>

#include "common/win32/hr_exception.h"

using namespace std;
using namespace common::win32;

WASAPIRenderDevice::WASAPIRenderDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : WASAPIDevice(descriptor, bufferTime)
{
}

WASAPIRenderDevice::~WASAPIRenderDevice()
{
}

void WASAPIRenderDevice::OnInitialize()
{
    try
    {
        _hr = CoInitialize(nullptr);
        _hr = InitializeAudioClient(&m_audioClient, m_audioFormat);

        m_logger.trace << "WASAPIRenderDevice::OnInitialize: " << m_audioFormat << endl;
    }
    catch (hr_exception e)
    {
        m_logger.error << "WASAPIRenderDevice::OnInitialize: " << e.ErrorMessage() << endl;
        throw;
    }
}

bool WASAPIRenderDevice::OnAddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block)
{
    lock_guard<mutex> lock(m_critSec);
    m_queue.push(block->buffer());
    return true;
}

void WASAPIRenderDevice::OnValidateFormat(const AudioFormat& format)
{
    if (format == INVALID_AUDIO_FORMAT)
        throw exception("AudioFormat is not supported");
}

void WASAPIRenderDevice::OnThreadProc()
{
    try
    {
        UINT32 maxSamplesCount;
        CComPtr<IAudioRenderClient> renderClient;
        _hr = m_audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));
        _hr = m_audioClient->GetBufferSize(&maxSamplesCount);

        _hr = m_audioClient->Start();

        while (!CheckClosing())
        {
            shared_ptr<Buffer> buffer;

            {
                lock_guard<mutex> lock(m_critSec);

                if (m_queue.size() != 0)
                {
                    buffer = m_queue.front();
                    m_queue.pop();
                }
            }

            if (!buffer)
            {
                Sleep(5);
                continue;
            }

            auto pSourceBuffer = buffer->data();
            auto sourceSamples = buffer->size() / m_audioFormat.blockAlign();

            while (sourceSamples != 0)
            {
                UINT32 samplesPadding;
                _hr = m_audioClient->GetCurrentPadding(&samplesPadding);
                auto targetSamples = maxSamplesCount - samplesPadding;

                auto samplesToProcess = min(sourceSamples, targetSamples);
                auto sizeToProcess = samplesToProcess * m_audioFormat.blockAlign();

                if (sizeToProcess == 0)
                {
                    Sleep(targetSamples / m_audioFormat.samplesPerSec() / 1000 / 2);
                    continue;
                }

                BYTE* pTargetBuffer;
                _hr = renderClient->GetBuffer(samplesToProcess, &pTargetBuffer);

                memcpy(pTargetBuffer, pSourceBuffer, sizeToProcess);

                sourceSamples -= samplesToProcess;
                pSourceBuffer += sizeToProcess;

                _hr = renderClient->ReleaseBuffer(samplesToProcess, 0);
            }
        }

        _hr = m_audioClient->Stop();
    }
    catch (hr_exception e)
    {
        cout << "WASAPIRenderDevice::OnThreadProc: " << e.ErrorMessage();
    }
}
