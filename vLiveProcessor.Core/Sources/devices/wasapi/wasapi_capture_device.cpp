#include "wasapi_capture_device.h"

#include "../../common/errors.h"

#include <iostream>

using namespace std;

WASAPICaptureDevice::WASAPICaptureDevice(DeviceInfo& deviceInfo, uint64_t bufferTime)
    : WASAPIDevice(deviceInfo, bufferTime)
{
}

WASAPICaptureDevice::~WASAPICaptureDevice()
{
}

HRESULT WASAPICaptureDevice::CaptureBuffer(shared_ptr<Buffer>& buffer)
{
    lock_guard<mutex> lock(m_critSec);

    if (m_queue.size())
    {
        buffer = m_queue.front();
        m_queue.pop();
        return S_OK;
    }

    return S_FALSE;
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

        cout << pWaveFormat->nChannels << " " << pWaveFormat->wBitsPerSample << " " << pWaveFormat->nSamplesPerSec << endl;

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
                        cout << availableFrames << " frames were dropped" << endl;
                        break;
                    }

                    auto actualBufferSize = buffer->GetMaxSize() - buffer->GetSize();
                    auto actualFramesCount = actualBufferSize / pWaveFormat->nBlockAlign;

                    auto framesToCopy = min(availableFrames, actualFramesCount);
                    auto sizeToCopy = framesToCopy * pWaveFormat->nBlockAlign;

                    if (silence)
                        memset(buffer->GetPointer() + buffer->GetSize(), 0, sizeToCopy);
                    else
                        memcpy(buffer->GetPointer() + buffer->GetSize(), pData, sizeToCopy);


                    pData += sizeToCopy;
                    availableFrames -= framesToCopy;

                    buffer->SetSize(buffer->GetSize() + sizeToCopy);

                    if (buffer->GetSize() == buffer->GetMaxSize())
                    {
                        lock_guard<mutex> lock(m_critSec);
                        m_queue.push(buffer);
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
        wcout << "WASAPICaptureDevice::OnThreadProc: " << e.ErrorMessage();
    }

    if (pWaveFormat)
        CoTaskMemFree(pWaveFormat);
}
