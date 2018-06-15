#pragma once

using namespace Collections::Generic;

public ref class DeviceEnumerator 
{
public:
    DeviceEnumerator()
    {
        m_pManager = new DeviceManager();
    }

    ~DeviceEnumerator()
    {
        delete m_pManager;
    }

    IEnumerable<DeviceGroup^>^ EnumrateGroups()
    {
        auto groups = gcnew List<DeviceGroup^>();

        for each (auto group in m_pManager->GetGroups())
        {
            groups->Add(gcnew DeviceGroup(gcnew Func<IntPtr>(this, &DeviceEnumerator::GetManager), gcnew String(group.c_str())));
        }

        return groups;
    }

internal:
    IntPtr GetManager()
    {
        return IntPtr(m_pManager);
    }

private:
    IDeviceManager *m_pManager;
};
