#pragma once
#include "wasapi_device.h"

#include <atlcomcli.h>

#include "common/threading/queue.h"
#include "media/interfaces/i_consumer.h"

class WASAPIRenderDevice : public WASAPIDevice
{
public:
    explicit WASAPIRenderDevice(DeviceDescriptor& descriptor);
    virtual ~WASAPIRenderDevice();

protected:
    void OnInitialize() override;
    bool OnAddBlock(std::chrono::nanoseconds timeout, std::shared_ptr<MediaBlock> block) override;
    void OnValidateFormat(const AudioFormat& format) override;

private:
    void OnThreadProc() override;

private:
    CComPtr<IAudioClient>                                               m_audioClient;
    common::threading::Queue<std::shared_ptr<common::memory::Buffer>>   m_queue;
    
};
