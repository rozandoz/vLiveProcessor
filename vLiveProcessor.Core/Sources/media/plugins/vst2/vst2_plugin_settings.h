#pragma once

#include <memory>
#include <string>

#include "media/interfaces/i_plugin_window_controller.h"

struct VST2PluginSettings
{
    VST2PluginSettings()
        : blockSize(1024)
        , sampleRate(48000)
        , windowController(nullptr)
    {
    }

    size_t                                      blockSize;
    size_t                                      sampleRate;
    std::string                                 modulePath;
    std::shared_ptr<IPluginWindowController>    windowController;
};
