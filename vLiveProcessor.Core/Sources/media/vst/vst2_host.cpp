#include "vst2_host.h"

#include "media/vst/vst2_plugin.h"

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

using namespace std;

shared_ptr<VST2Host> VST2Host::Create()
{
    return make_shared<VST2Host>();
}

VST2Host::~VST2Host()
{
    StopThread();
}

shared_ptr<VST2Plugin> VST2Host::CreatePlugin(const VST2PluginSettings& settings)
{
    if (!CheckStarted()) StartThread();
    return make_shared<VST2Plugin>(settings, shared_from_this());
}

bool VST2Host::EventRoutine()
{
#ifdef WIN32
    WaitMessage();
    MSG message = {};
    if (!PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        return false;

    TranslateMessage(&message);
    DispatchMessage(&message);

    return true;
#else // WIN32
    throw new exception("Not implemented");
#endif
}

void VST2Host::OnThreadProc()
{
    while (!CheckClosing()) 
    {
        if (!EventRoutine()) 
            break;

        this_thread::sleep_for(1ms);
    }
}
