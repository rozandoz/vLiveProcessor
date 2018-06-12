// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <algorithm>

#include "common/logging/logger.h"

#include "media/devices/device_manager.h"
#include "media/plugins/plugin_manager.h"
#include "media/audio_bus/audio_bus.h"

#include "TestWindowController.h"

using namespace std;

int main()
{
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

    auto& logger = Logger::GetInstance();

    try
    {
        auto controller = make_shared<TestWindowController>();

        auto deviceProvider = make_unique<DeviceManager>();

        auto captureDevices = deviceProvider->GetGroupDevices("WASAPI", Capture);
        auto renderDevices = deviceProvider->GetGroupDevices("WASAPI", Render);

        logger.trace << "Capture devices: " << endl;
        for_each(captureDevices.begin(), captureDevices.end(), [&](DeviceDescriptor d)-> void { logger.trace << "\t" << d.name << endl; });

        logger.trace << "Render devices: " << endl;
        for_each(renderDevices.begin(), renderDevices.end(), [&](DeviceDescriptor d)-> void { logger.trace << "\t" << d.name << endl; });

        logger.trace << "-----------------------------------------" << endl;

        auto pluginProvider = make_unique<PluginManager>();

        auto vst2Plugins = pluginProvider->GetGroupPlugins("VST2");

        logger.trace << "Plugins: " << endl;
        for_each(vst2Plugins.begin(), vst2Plugins.end(), [&](PluginDescriptor d)-> void { logger.trace << "\t" << d.name << " (" << d.location << ")" << endl; });

        logger.trace << endl;
        logger.trace << "Initialize AudioBus" << endl;

        vector<shared_ptr<IProcessor>> processors;
        processors.push_back(deviceProvider->CreateDevice("WASAPI", Capture, captureDevices[0]));
        processors.push_back(pluginProvider->CreatePlugin("VST2", vst2Plugins[0], PluginSettings { controller }));
        processors.push_back(deviceProvider->CreateDevice("WASAPI", Render, renderDevices[0]));

        auto audiobus = make_unique<AudioBus>();

        audiobus->SetProcessors(processors);
        audiobus->Start();

        while (true)
        {
            if (GetAsyncKeyState(0x58)) // X
            {
                logger.trace << "Closing..." << endl;
                break;
            }

            WaitMessage();

            MSG message = {};
            if (!PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                continue;

            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        audiobus->Stop();
    }
    catch (exception e)
    {
        logger.error << e.what() << endl;
    }

    return 0;
}
