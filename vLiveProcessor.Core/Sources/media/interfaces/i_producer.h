#pragma once

#include "../media_block.h"

class IProducer
{
protected:
    virtual ~IProducer() {}

public:
    virtual bool TryGetBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) = 0;
};