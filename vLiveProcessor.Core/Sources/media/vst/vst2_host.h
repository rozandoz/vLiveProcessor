#pragma once

#include <string>
#include <memory>
#include <vector>

#include "common/threading/thread_base.h"
#include "media/vst/vst2_plugin_settings.h"

class VST2Plugin;

class VST2Host : public ThreadBase, public std::enable_shared_from_this<VST2Host>
{
    friend class std::_Ref_count_obj<VST2Host>;

public:
    static std::shared_ptr<VST2Host> Create();

private:
    VST2Host();
    virtual ~VST2Host();

public:
    std::shared_ptr<VST2Plugin> CreatePlugin(const VST2PluginSettings& settings);

    std::string Vendor() { return "VENDOR"; }
    std::string Product() { return "PRODUCT"; }
    int VendorVersion() { return 1; }

    std::vector<std::string> Capabilities() 
    {
        std::vector<std::string> capabilities;

        //"sendVstEvents",
        //"sendVstMidiEvents"
        //"sendVstMidiEventFlagIsRealtime"
        capabilities.push_back("sizeWindow");
        capabilities.push_back("startStopProcess");

        return  Capabilities();
    }

private:
    bool EventRoutine();
    void OnThreadProc() override;
};
