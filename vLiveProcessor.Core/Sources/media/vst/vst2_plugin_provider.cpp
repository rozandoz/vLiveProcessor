#include "vst2_plugin_provider.h"

#include <memory>

#include "media/vst/vst2_processor.h"

using namespace std;

VST2PluginProvider::VST2PluginProvider()
{
}

VST2PluginProvider::~VST2PluginProvider()
{
}

string VST2PluginProvider::Group()
{
    return "VST2";
}

vector<PluginDescriptor> VST2PluginProvider::EnumeratePlugins()
{
    vector<PluginDescriptor> descriptors;

    PluginDescriptor descriptor;
    descriptor.name = "TDR Nova";
    descriptor.location = "E:\\temp\\TDR Nova (no installer)\\VST2\\x64\\TDR Nova.dll";

    descriptors.push_back(descriptor);
    return descriptors;
}

shared_ptr<IProcessor> VST2PluginProvider::CreatePlugin(const PluginDescriptor& descriptor, const PluginSettings& settings)
{
    VST2PluginSettings vst2Settings;
    vst2Settings.modulePath = descriptor.location;
    vst2Settings.window = settings.pWindow;
    return make_shared<VST2Processor>(vst2Settings);
}
