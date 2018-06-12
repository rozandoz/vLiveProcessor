#pragma once

#include <memory>
#include <string>

#include "media/audio_format.h"
#include "media/interfaces/i_plugin_window_controller.h"

struct VST2PluginSettings
{
    VST2PluginSettings()
        : blockSize(1024)
        , windowController(nullptr)
    {
    }

    size_t                                      blockSize;
    AudioFormat                                 audioFormat;
    std::string                                 modulePath;
    std::shared_ptr<IPluginWindowController>    windowController;
};
