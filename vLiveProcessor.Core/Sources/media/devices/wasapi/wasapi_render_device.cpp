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

bool WASAPIRenderDevice::TryPushBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block)
{
    lock_guard<mutex> lock(m_critSec);
    m_queue.push(block->buffer());
    return true;
}

void WASAPIRenderDevice::OnThreadProc()
{
    try
    {
        _hr = CoInitialize(nullptr);

        AudioFormat fmt;
        UINT32 maxSamplesCount;

        CComPtr<IAudioClient> client;
        CComPtr<IAudioRenderClient> renderClient;
        _hr = InitializeAudioClient(&client, fmt);
        _hr = client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));
        _hr = client->GetBufferSize(&maxSamplesCount);

        //cout << pWaveFormat->nChannels << " " << pWaveFormat->wBitsPerSample << " " << pWaveFormat->nSamplesPerSec << endl;

        _hr = client->Start();

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
            auto sourceSamples = buffer->size() / fmt.blockAlign();

            while (sourceSamples != 0)
            {
                UINT32 samplesPadding;
                _hr = client->GetCurrentPadding(&samplesPadding);
                auto targetSamples = maxSamplesCount - samplesPadding;

                auto samplesToProcess = min(sourceSamples, targetSamples);
                auto sizeToProcess = samplesToProcess * fmt.blockAlign();

                if (sizeToProcess == 0)
                {
                    Sleep(targetSamples / fmt.samplesPerSec() / 1000 / 2);
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

        _hr = client->Stop();
    }
    catch (hr_exception e)
    {
        cout << "WASAPIRenderDevice::OnThreadProc: " << e.ErrorMessage();
    }
}
