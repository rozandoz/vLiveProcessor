#pragma once

using namespace Collections::Generic;

public ref class DeviceGroup
{
internal:
    DeviceGroup(Func<IntPtr>^ manager, String^ group)
    {
        m_manager = manager;
        m_group = group;
    }

public:
    property String^ Name
    {
        String^ get() { return m_group; }
    }

    IEnumerable<Device^>^ EnumberateDevice()
    {
        auto manager = (IDeviceManager*)m_manager->Invoke().ToPointer();
        auto groupName = Strings::MarshalString(m_group);
        
        auto devices = gcnew List<Device^>();
        
        for each (auto device in manager->GetGroupDevices(groupName, DeviceType::Capture))
            devices->Add(gcnew Device(device, DeviceType::Capture));

        for each (auto device in manager->GetGroupDevices(groupName, DeviceType::Render))
            devices->Add(gcnew Device(device, DeviceType::Render));

        return devices;
    }

private:
    Func<IntPtr>^       m_manager;
    String^             m_group;
};