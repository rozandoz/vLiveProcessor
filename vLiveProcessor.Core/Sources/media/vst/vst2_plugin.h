#pragma once

#include <string>

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

    void AllocateBuffrers();
    void Initialize();
    void Close();

    VstIntPtr HostCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float)
    {
        switch (opcode)
        {
        default:                                break;
        case audioMasterVersion:                return kVstVersion;
        case audioMasterCurrentId:              return m_pEffect->uniqueID;
        case audioMasterGetSampleRate:          return m_settings.sampleRate;
        case audioMasterGetBlockSize:           return m_settings.blockSize;
        case audioMasterGetCurrentProcessLevel: return kVstProcessLevelUnknown;
        case audioMasterGetAutomationState:     return kVstAutomationOff;
        case audioMasterGetLanguage:            return kVstLangEnglish;
        case audioMasterGetVendorVersion:       return m_host->VendorVersion();

        case audioMasterGetVendorString:
            strcpy_s(static_cast<char*>(ptr), kVstMaxVendorStrLen, m_host->Vendor().c_str());
            return 1;

        case audioMasterGetProductString:
            strcpy_s(static_cast<char*>(ptr), kVstMaxProductStrLen, m_host->Product().c_str());
            return 1;

            /*  case audioMasterGetTime:
                  VstTimeInfo timeinfo;
                  timeinfo.flags = 0;
                  timeinfo.samplePos = 0;
                  timeinfo.sampleRate = m_settings.sampleRate;
                  return reinterpret_cast<VstIntPtr>(&timeinfo);

              case audioMasterGetDirectory:
                  return reinterpret_cast<VstIntPtr>(L"E:\\temp\\Synth1");*/

        case audioMasterIdle:
            if (m_settings.window)
            {
                Dispatcher(effEditIdle);
            }
            break;

        case audioMasterSizeWindow:
            if (m_settings.window) {
                RECT rc{};
                GetWindowRect((HWND)m_settings.window, &rc);
                rc.right = rc.left + static_cast<int>(index);
                rc.bottom = rc.top + static_cast<int>(value);
                ResizeEditor(rc);
            }
            break;

        case audioMasterCanDo:
            for each (auto capability in m_host->Capabilities())
            {
                if (strcmp(capability.c_str(), static_cast<const char*>(ptr)) == 0)
                    return 1;
            }
            return 0;
        }
        return 0;
    }

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
