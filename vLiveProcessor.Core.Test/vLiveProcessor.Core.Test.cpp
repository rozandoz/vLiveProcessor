// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <memory>

#include "common/logging/logger.h"
#include "common/win32/strings_converter.h"
#include "devices/device_manager.h"

using namespace std;

int main()
{
    auto provider = make_unique<DeviceManager>();
    auto groups = provider->GetGroups();

    for (auto group : groups)
    {
        cout << "'" << group << "' group" << endl;

        auto devices = provider->GetGroupDevices(group, Render);

        for (auto device : devices)
        {
            cout << "\t " << device.name << " (" << device.id << ")" << endl;
        }
    }

     getchar();

    return 0;
}
