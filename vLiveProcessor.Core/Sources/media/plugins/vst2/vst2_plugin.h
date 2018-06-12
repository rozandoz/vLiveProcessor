#pragma once

#include "pluginterfaces/vst2.x/aeffectx.h"

#include "media/plugins/vst2/vst2_plugin_settings.h"
#include "media/plugins/vst2/vst2_module.h"

#include "common/logging/logger.h"

class VST2Plugin
{
    static VstIntPtr VSTCALLBACK StaticCallback(AEffect * effect,
        VstInt32 opcode,
        VstInt32 index,
        VstIntPtr value,
        void * ptr,
        float opt);

    static std::vector<std::string> Capabilities();


public:
    VST2Plugin(const VST2PluginSettings& settings);
    ~VST2Plugin();

protected:
    //Helpers
    bool HasFlag(int32_t flag) const;
    bool HasEditor() const;

    intptr_t Dispatcher(int32_t opcode, int32_t index = 0, intptr_t value = 0, void *ptr = nullptr, float opt = 0.0f) const;
    VstIntPtr HostCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float) const;

    void AllocateBuffrers();
    void Initialize();
    void Close();

public:
    size_t ProcessInterlaved(float* pInBuffer, float* pOutBuffer, size_t inputFrames);

private:
    Logger&                                     m_logger;

    size_t                                      m_blockSize;
    AudioFormat                                 m_audioFormat;
    std::string                                 m_modulePath;
    std::shared_ptr<IPluginWindowController>    m_windowController;

    std::unique_ptr<VST2Module>                 m_module;
    AEffect*                                    m_pEffect;

    std::vector<float>                          m_inputBuffer;
    std::vector<float>                          m_outputBuffer;

    std::vector<float*>                         m_outputBufferPointers;
    std::vector<float*>                         m_inputBufferPointers;
};
