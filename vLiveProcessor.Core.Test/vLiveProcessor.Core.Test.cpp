// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <algorithm>
#include <iostream>

#include "common/logging/logger.h"

#include "media/devices/device_manager.h"
#include "media/vst/vst2_processor.h"
#include "media/audio_bus/audio_bus.h"


using namespace std;

int main()
{
    auto& logger = Logger::GetInstance();

    auto deviceProvider = make_unique<DeviceManager>();

    auto captureDevices = deviceProvider->GetGroupDevices("WASAPI", Capture);
    auto renderDevices = deviceProvider->GetGroupDevices("WASAPI", Render);

    logger.trace << "Capture devices: " << endl;
    for_each(captureDevices.begin(), captureDevices.end(), [&](DeviceDescriptor d)-> void { logger.trace << "\t" << d.name << endl; });

    logger.trace << "Render devices: " << endl;
    for_each(renderDevices.begin(), renderDevices.end(), [&](DeviceDescriptor d)-> void { logger.trace << "\t" << d.name << endl; });

    logger.trace << endl;
    logger.trace << "Initialize AudioBus" << endl;

    VST2PluginSettings vst2Settings;
    vst2Settings.modulePath = "E:\\temp\\TDR Nova (no installer)\\VST2\\x64\\TDR Nova.dll";

    vector<shared_ptr<IProcessor>> processors;
    processors.push_back(deviceProvider->CreateDevice("WASAPI", Capture, captureDevices[0]));
    processors.push_back(make_shared<VST2Processor>(vst2Settings));
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

    return 0;
}
