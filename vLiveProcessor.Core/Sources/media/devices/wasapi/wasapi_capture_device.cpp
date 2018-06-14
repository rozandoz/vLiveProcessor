#include "wasapi_capture_device.h"

#include "common/win32/hr_exception.h"
#include "common/memory/ring_buffer.h"

using namespace std;
using namespace common::win32;
using namespace common::memory;

WASAPICaptureDevice::WASAPICaptureDevice(DeviceDescriptor& descriptor, uint64_t bufferTime)
    : WASAPIDevice(descriptor, bufferTime)
{
}

WASAPICaptureDevice::~WASAPICaptureDevice()
{
}

void WASAPICaptureDevice::OnInitialize()
{
    try
    {
        _hr = CoInitialize(nullptr);
        _hr = InitializeAudioClient(&m_audioClient, m_audioFormat);

        m_logger.trace << "WASAPICaptureDevice::OnInitialize: " << m_audioFormat << endl;

    }
    catch(hr_exception e)
    {
        m_logger.error << "WASAPICaptureDevice::OnInitialize: " << e.ErrorMessage() << endl;
        throw;
    }
}

void WASAPICaptureDevice::OnThreadProc()
{
    SetThreadPriority(GetNativeHandle(), THREAD_PRIORITY_TIME_CRITICAL);

    try
    {
        CComPtr<IAudioCaptureClient> captureClient;
        _hr = m_audioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&captureClient));
        
        auto allocator = RingBuffer::Create(maxBufferSamples() * m_audioFormat.blockAlign());

        _hr = m_audioClient->Start();

        shared_ptr<Buffer> buffer;

        while (!CheckClosing())
        {
            UINT32 packetLength = 0;
            _hr = captureClient->GetNextPacketSize(&packetLength);

            while (packetLength != 0 && !CheckClosing())
            {
                BYTE* pData;
                UINT availableFrames = 0;
                DWORD flags = 0;

                _hr = captureClient->GetBuffer(&pData, &availableFrames, &flags, nullptr, nullptr);

                auto silence = flags & AUDCLNT_BUFFERFLAGS_SILENT;
                auto samplesToRelease = availableFrames;

                while (availableFrames != 0)
                {
                    auto waitTime = AudioFormat::GetDurationNs(m_audioFormat, availableFrames);

                    if (buffer == nullptr && !allocator->TryGetBuffer(waitTime, availableFrames * m_audioFormat.blockAlign(), buffer))
                    {
                        m_logger.warning << "WASAPICaptureDevice: " << availableFrames << " frames were dropped" << endl;
                        break;
                    }

                    auto actualBufferSize = buffer->max_size() - buffer->size();
                    auto actualFramesCount = actualBufferSize / m_audioFormat.blockAlign();

                    auto framesToCopy = min(availableFrames, (UINT)actualFramesCount);
                    auto sizeToCopy = framesToCopy * m_audioFormat.blockAlign();

                    if (silence)
                        memset(buffer->data() + buffer->size(), 0, sizeToCopy);
                    else
                        memcpy(buffer->data() + buffer->size(), pData, sizeToCopy);


                    pData += sizeToCopy;
                    availableFrames -= framesToCopy;

                    buffer->set_size(buffer->size() + sizeToCopy);

                    if (buffer->size() == buffer->max_size())
                    {
                        if(m_consumer != nullptr)
                        {
                            if(!m_consumer->AddBlock(waitTime, make_shared<MediaBlock>(buffer, m_audioFormat)))
                                m_logger.error << "WASAPICaptureDevice: failed to push block to consumer" << endl;
                        }

                        buffer.reset();
                    }
                }

                _hr = captureClient->ReleaseBuffer(samplesToRelease);
                _hr = captureClient->GetNextPacketSize(&packetLength);
            }

            this_thread::sleep_for(1ms);
        }

        _hr = m_audioClient->Stop();
    }
    catch (hr_exception e)
    {
        m_logger.error << "WASAPICaptureDevice::OnThreadProc: " << e.ErrorMessage();
    }
}