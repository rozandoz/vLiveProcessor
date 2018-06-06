#pragma once

#include <string>
#include <algorithm>

#include "pluginterfaces/vst2.x/aeffectx.h"

#include "media/vst/vst2_host.h"

#include "media/vst/vst2_plugin_settings.h"
#include "media/vst/vst2_module.h"

#include "common/logging/logger.h"

class VST2Plugin
{
    static VstIntPtr VSTCALLBACK StaticCallback(AEffect * effect,
        VstInt32 opcode,
        VstInt32 index,
        VstIntPtr value,
        void * ptr,
        float opt);

public:
    VST2Plugin(const VST2PluginSettings& settings, std::shared_ptr<VST2Host> host);
    ~VST2Plugin();

protected:
    //Temp
    void ResizeEditor(const RECT& clientRc) const;

    //Helpers
    bool HasFlag(int32_t flag) const;
    bool HasEditor() const;

    intptr_t Dispatcher(int32_t opcode, int32_t index = 0, intptr_t value = 0, void *ptr = nullptr, float opt = 0.0f) const;
    VstIntPtr HostCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float) const;

    void AllocateBuffrers();
    void Initialize();
    void Close();

public:
    std::vector<float*>& Process(float* pBuffer, uint32_t& frames);

private:
    Logger&                     m_logger;
    VST2PluginSettings          m_settings;
    std::shared_ptr<VST2Host>   m_host;

    std::unique_ptr<VST2Module> m_module;
    AEffect*                    m_pEffect;

    std::vector<float>          m_inputBuffer;
    std::vector<float>          m_outputBuffer;

    std::vector<float*>         m_outputBufferPointers;
    std::vector<float*>         m_inputBufferPointers;
};
