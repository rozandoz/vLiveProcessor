#include "wasapi_render_device.h"

#include <iostream>

#include "common/win32/hr_exception.h"

using namespace std;
using namespace chrono;
using namespace common::win32;
using namespace common::memory;

WASAPIRenderDevice::WASAPIRenderDevice(DeviceDescriptor& descriptor)
    : WASAPIDevice(descriptor)
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

        m_logger.trace << _log_call_pref_ << m_audioFormat << endl;
    }
    catch (hr_exception e)
    {
        m_logger.error << _log_call_pref_ << e.ErrorMessage() << endl;
        throw;
    }
}

bool WASAPIRenderDevice::OnAddBlock(nanoseconds timeout, shared_ptr<MediaBlock> block)
{
    return m_queue.TryAdd(timeout, block->buffer());
}

void WASAPIRenderDevice::OnValidateFormat(const AudioFormat& format)
{
    if (format == INVALID_AUDIO_FORMAT)
        throw exception("AudioFormat is not supported");
}

void WASAPIRenderDevice::OnThreadProc()
{
    SetThreadPriority(GetNativeHandle(), THREAD_PRIORITY_TIME_CRITICAL);

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
            if(!m_queue.TryGet(1ms, buffer))
                continue;

            auto pSourceBuffer = buffer->data();
            auto sourceSamples = buffer->size() / m_audioFormat.blockAlign();

            while (sourceSamples != 0)
            {
                UINT32 samplesPadding;
                _hr = m_audioClient->GetCurrentPadding(&samplesPadding);
                auto targetSamples = maxSamplesCount - samplesPadding;

                auto samplesToProcess = UINT32(min(sourceSamples, targetSamples));
                auto sizeToProcess = samplesToProcess * m_audioFormat.blockAlign();

                if (sizeToProcess == 0)
                {
                    this_thread::sleep_for(1ms);
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
