#pragma once

#include "i_consumer.h"

class IProducer
{
protected:
    virtual ~IProducer() {}

public:
    virtual void SetConsumer(const std::shared_ptr<IConsumer>& consumer) = 0;
};