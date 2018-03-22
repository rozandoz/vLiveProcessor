#pragma once

class IDevice
{
protected:
    IDevice() { }
    virtual ~IDevice() { }

public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Reset() = 0;
};