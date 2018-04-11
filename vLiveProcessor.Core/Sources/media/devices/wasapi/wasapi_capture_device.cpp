#include "wasapi_capture_device.h"

#include <iostream>

#include "win32/hr_exception.h"

using namespace std;
using namespace common::win32;

WASAPICaptureDevice::WASAPICaptureDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : WASAPIDevice(descriptor, bufferTime)
{
}

WASAPICaptureDevice::~WASAPICaptureDevice()
{
}

void WASAPICaptureDevice::Callback(const shared_ptr<IProducerCallback>& callback)
{
    m_callback = callback;
}

void WASAPICaptureDevice::OnThreadProc()
{
    WAVEFORMATEX* pWaveFormat;

    try
    {
        _hr = CoInitialize(nullptr);

        UINT32 maxSamplesCount;

        CComPtr<IAudioClient> client;
        CComPtr<IAudioCaptureClient> captureClient;

        _hr = InitializeAudioClient(&client, &pWaveFormat);
        _hr = client->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&captureClient));
        _hr = client->GetBufferSize(&maxSamplesCount);

        AudioFormat audioFormat(pWaveFormat->nChannels, pWaveFormat->wBitsPerSample, pWaveFormat->nSamplesPerSec);

        m_logger.trace << "WASAPICaptureDevice: " << audioFormat << endl;

        auto maxBufferSize = maxSamplesCount * pWaveFormat->nBlockAlign;
        auto allocator = MemoryAllocator::Create(maxBufferSize / BUFFERS_COUNT, BUFFERS_COUNT);

        auto actualDuration = static_cast<double>(REFTIMES_PER_SEC) * maxSamplesCount / pWaveFormat->nSamplesPerSec;
        auto waitTime = actualDuration / REFTIMES_PER_MILLISEC / 2;

        _hr = client->Start();

        shared_ptr<Buffer> buffer;

        while (!CheckClosing())
        {
            Sleep(waitTime);

            UINT32 packetLength = 0;
            _hr = captureClient->GetNextPacketSize(&packetLength);

            while (packetLength != 0 && !CheckClosing())
            {
                BYTE* pData;
                UINT availableFrames = 0;
                DWORD flags = 0;

                UINT64 position;
                _hr = captureClient->GetBuffer(&pData, &availableFrames, &flags, &position, nullptr);

                auto silence = flags & AUDCLNT_BUFFERFLAGS_SILENT;
                auto samplesToRelease = availableFrames;

                while (availableFrames != 0)
                {
                    if (buffer == nullptr && !allocator->TryGetBuffer(waitTime, buffer))
                    {
                        m_logger.warning << "WASAPICaptureDevice: " << availableFrames << " frames were dropped!" << endl;
                        break;
                    }

                    auto actualBufferSize = buffer->max_size() - buffer->size();
                    auto actualFramesCount = actualBufferSize / pWaveFormat->nBlockAlign;

                    auto framesToCopy = min(availableFrames, actualFramesCount);
                    auto sizeToCopy = framesToCopy * pWaveFormat->nBlockAlign;

                    if (silence)
                        memset(buffer->data() + buffer->size(), 0, sizeToCopy);
                    else
                        memcpy(buffer->data() + buffer->size(), pData, sizeToCopy);


                    pData += sizeToCopy;
                    availableFrames -= framesToCopy;

                    buffer->set_size(buffer->size() + sizeToCopy);

                    if (buffer->size() == buffer->max_size())
                    {
                        if(m_callback != nullptr)
                        {
                            m_callback->OnProcessBlock(make_shared<MediaBlock>(buffer, audioFormat));
                        }

                        buffer.reset();
                    }
                }

                _hr = captureClient->ReleaseBuffer(samplesToRelease);
                _hr = captureClient->GetNextPacketSize(&packetLength);
            }
        }

        _hr = client->Stop();
    }
    catch (hr_exception e)
    {
        m_logger.error << "WASAPICaptureDevice::OnThreadProc: " << e.ErrorMessage();
    }

    if (pWaveFormat)
        CoTaskMemFree(pWaveFormat);
}
