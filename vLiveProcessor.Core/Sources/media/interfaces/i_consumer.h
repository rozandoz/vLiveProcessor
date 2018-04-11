#pragma once

#include "media/media_block.h"

class IConsumer
{
protected:
    virtual ~IConsumer() {}

public:
    virtual bool TryPushBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) = 0;
};