// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <algorithm>

#include "common/logging/logger.h"
#include "media/devices/device_manager.h"
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

    vector<shared_ptr<IProcessor>> processors;
    processors.push_back(deviceProvider->CreateDevice("WASAPI", Capture, captureDevices[0]));
    processors.push_back(deviceProvider->CreateDevice("WASAPI", Render, renderDevices[0]));

    auto audiobus = make_unique<AudioBus>();
    
    audiobus->SetProcessors(processors);
    audiobus->Start();

    getchar();

    audiobus->Stop();

    return 0;
}
