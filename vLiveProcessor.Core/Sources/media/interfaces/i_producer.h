#pragma once

#include "../media_block.h"

class IProducerCallback
{
protected:
    virtual ~IProducerCallback() {}

public:
    virtual void OnProcessBlock(const std::shared_ptr<MediaBlock>& block) = 0;
};

class IProducer
{
protected:
    virtual ~IProducer() {}

public:
    virtual void Callback(const std::shared_ptr<IProducerCallback>& callback) = 0;
};