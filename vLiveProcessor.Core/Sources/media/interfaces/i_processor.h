#pragma once

class IProcessor
{
protected:
    virtual ~IProcessor() {}

public:
    virtual void Initialize() = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Reset() = 0;
};