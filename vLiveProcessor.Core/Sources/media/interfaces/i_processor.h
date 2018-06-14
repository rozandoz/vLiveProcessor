#pragma once

struct ProcessorSettings
{
    ProcessorSettings(size_t maxBufferSamples = 1024)
        : maxBufferSamples(maxBufferSamples)
    {
    }
     
    size_t maxBufferSamples;
};

class IProcessor
{
protected:
    virtual ~IProcessor() {}

public:
    virtual void Initialize(const ProcessorSettings& settings) = 0;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Reset() = 0;
};