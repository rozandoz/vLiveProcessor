#pragma once

#include <memory>
#include <vector>

#include "common/logging/logger.h"

#include "media/interfaces/i_device.h"

class AudioBus
{
    typedef std::vector<std::shared_ptr<IProcessor>> ProcessorsCollection;

public:
    AudioBus();
    virtual ~AudioBus();

    void SetProcessors(ProcessorsCollection& processors);
    
    void Start();
    void Stop();

private:
    ProcessorsCollection    m_processors;
    Logger&                 m_logger;
    
    std::mutex              m_critSec;
    bool                    m_started;
};
