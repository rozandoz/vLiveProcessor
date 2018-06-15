#pragma once

using namespace System;

public ref class Device
{
internal:
    Device(const DeviceDescriptor& descriptor, const DeviceType& type)
    {
        m_pDescriptor = new DeviceDescriptor(descriptor);
        m_deviceType = type;
    }

    ~Device()
    {
        delete m_pDescriptor;
    }

    DeviceDescriptor* GetDescriptor()
    {
        return m_pDescriptor;
    }

public:
    property String^ Name
    {
        String^ get() { return gcnew String(m_pDescriptor->name.c_str()); }
    }

    property String^ Id
    {
        String^ get() { return gcnew String(m_pDescriptor->id.c_str()); }
    }

private:
    DeviceDescriptor *m_pDescriptor;
    DeviceType        m_deviceType;
};