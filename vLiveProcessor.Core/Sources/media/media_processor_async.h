#pragma once

#include "common/threading/thread_base.h"
#include "media/media_processor.h"

class MediaProcessorAsync : public MediaProcessor, protected ThreadBase
{
public:
    MediaProcessorAsync();
    virtual ~MediaProcessorAsync();

protected:
    void OnStart() override;
    void OnStop() override;
    void OnReset() override;
};
