#pragma once

#include <string>

struct VST2PluginSettings
{
    VST2PluginSettings()
        : sampleRate(48000)
        , channelCount(2)
        , blockSize(1024)
    {
    }

    std::string     modulePath;

    size_t          sampleRate;
    size_t          channelCount;
    size_t          blockSize;

    void            *window = nullptr;
};
