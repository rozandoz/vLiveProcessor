#pragma once

struct ProcessorSettings
{
    ProcessorSettings(size_t bufferSamples = 48, size_t buffersCount = 20)
        : bufferSamples(bufferSamples)
        , buffersCount(buffersCount)
    {
    }
     
    size_t bufferSamples;
    size_t buffersCount;
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