#pragma once

#include "media/media_block.h"

class IConsumer
{
protected:
    virtual ~IConsumer() {}

public:
    virtual void SetAudioFormat(const AudioFormat& audioFormat) = 0;
    virtual bool AddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) = 0;
};