#include "wasapi_render_device.h"

#include <iostream>

#include "win32/hr_exception.h"

using namespace std;
using namespace common::win32;

WASAPIRenderDevice::WASAPIRenderDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : WASAPIDevice(descriptor, bufferTime)
{
}

WASAPIRenderDevice::~WASAPIRenderDevice()
{
}

HRESULT WASAPIRenderDevice::RenderBuffer(shared_ptr<Buffer> buffer)
{
    try
    {
        lock_guard<mutex> lock(m_critSec);
        m_queue.push(buffer);
    }
    catch (hr_exception ex)
    {
        return ex.Error();
    }

    return S_OK;
}

void WASAPIRenderDevice::OnThreadProc()
{
    WAVEFORMATEX* pWaveFormat;

    try
    {
        _hr = CoInitialize(nullptr);

        UINT32 maxSamplesCount;

        CComPtr<IAudioClient> client;
        CComPtr<IAudioRenderClient> renderClient;
        _hr = InitializeAudioClient(&client, &pWaveFormat);
        _hr = client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderClient));
        _hr = client->GetBufferSize(&maxSamplesCount);

        cout << pWaveFormat->nChannels << " " << pWaveFormat->wBitsPerSample << " " << pWaveFormat->nSamplesPerSec << endl;

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
            auto sourceSamples = buffer->size() / pWaveFormat->nBlockAlign;

            while (sourceSamples != 0)
            {
                UINT32 samplesPadding;
                _hr = client->GetCurrentPadding(&samplesPadding);
                auto targetSamples = maxSamplesCount - samplesPadding;

                auto samplesToProcess = min(sourceSamples, targetSamples);
                auto sizeToProcess = samplesToProcess * pWaveFormat->nBlockAlign;

                if (sizeToProcess == 0)
                {
                    Sleep(targetSamples / pWaveFormat->nSamplesPerSec / 1000 / 2);
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

    if (pWaveFormat)
        CoTaskMemFree(pWaveFormat);
}
