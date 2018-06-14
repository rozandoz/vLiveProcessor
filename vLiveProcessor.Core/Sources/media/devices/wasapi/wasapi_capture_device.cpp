#include "wasapi_capture_device.h"

#include "common/win32/hr_exception.h"
#include "common/memory/ring_buffer.h"

using namespace std;
using namespace common::win32;
using namespace common::memory;

WASAPICaptureDevice::WASAPICaptureDevice(DeviceDescriptor& descriptor)
    : WASAPIDevice(descriptor)
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

        m_logger.trace << _log_call_pref_ << m_audioFormat << endl;

    }
    catch(hr_exception e)
    {
        m_logger.error << _log_call_pref_ << e.ErrorMessage() << endl;
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

        m_logger.trace << _log_call_pref_ << "buffer size - " << maxBufferSamples() << " samples" << endl;

        _hr = m_audioClient->Start();

        this_thread::sleep_for(AudioFormat::GetDurationNs(m_audioFormat, maxBufferSamples()));

        while (!CheckClosing())
        {
            UINT32 packetLength = 0;
            _hr = captureClient->GetNextPacketSize(&packetLength);

            if(packetLength != 0)
            {
                BYTE* pData;
                DWORD flags = 0;
                UINT availableFrames = 0;

                _hr = captureClient->GetBuffer(&pData, &availableFrames, &flags, nullptr, nullptr);

                auto waitTime = AudioFormat::GetDurationNs(m_audioFormat, availableFrames);
                auto availableSize = availableFrames * m_audioFormat.blockAlign();

                shared_ptr<Buffer> buffer;
                if (allocator->TryGetBuffer(waitTime, availableSize, buffer))
                {
                    
                    if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                        memset(buffer->data(), 0, availableSize);
                    else
                        memcpy(buffer->data(), pData, availableSize);

                    buffer->set_size(availableSize);

                    if (!m_consumer->AddBlock(waitTime, make_shared<MediaBlock>(buffer, m_audioFormat)))
                        m_logger.error << _log_call_pref_ << " failed to push block to consumer" << endl;
                }
                else
                {
                    m_logger.warning << _log_call_pref_ << availableFrames << " samples have been dropped!" << endl;
                }

                _hr = captureClient->ReleaseBuffer(availableFrames);
            }

            this_thread::sleep_for(1ms);
        }

        _hr = m_audioClient->Stop();
    }
    catch (hr_exception e)
    {
        m_logger.error << _log_call_pref_ << e.ErrorMessage();
    }
}