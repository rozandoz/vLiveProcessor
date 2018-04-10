#pragma once

class IDevice
{
protected:
    virtual ~IDevice() {}

public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Reset() = 0;
};



