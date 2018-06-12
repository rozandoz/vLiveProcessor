#include "vst2_plugin.h"

#include <cassert>
#include <algorithm>

#include "media/plugins/vst2/vst_constants.h"

using namespace std;

VstIntPtr VSTCALLBACK VST2Plugin::StaticCallback(AEffect * effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void * ptr, float opt)
{
    if (effect && effect->user) {
        auto* that = static_cast<VST2Plugin*>(effect->user);
        return that->HostCallback(opcode, index, value, ptr, opt);
    }

    switch (opcode)
    {
    case audioMasterVersion:    return kVstVersion;
    default:                    return 0;
    }
}

vector<string> VST2Plugin::Capabilities()
{
    vector<string> capabilities =
    {
        //"sendVstEvents",
        //"sendVstMidiEvents"
        //"sendVstMidiEventFlagIsRealtime"
        "sizeWindow",
        "sizeWindow"
    };

    return capabilities;
}

VST2Plugin::VST2Plugin(const VST2PluginSettings& settings)
    : m_logger(Logger::GetInstance())
    , m_blockSize(settings.blockSize)
    , m_audioFormat(settings.audioFormat)
    , m_modulePath(settings.modulePath)
    , m_windowController(settings.windowController)
    , m_pEffect(nullptr)
{
    Initialize();
}

VST2Plugin::~VST2Plugin()
{
    Close();
}

bool VST2Plugin::HasFlag(int32_t flag) const
{
    assert(m_pEffect);
    return (m_pEffect->flags & flag) == flag;
}

bool VST2Plugin::HasEditor() const
{
    return HasFlag(effFlagsHasEditor) && m_windowController != nullptr;
}

intptr_t VST2Plugin::Dispatcher(int32_t opcode, int32_t index, intptr_t value, void * ptr, float opt) const
{
    assert(m_pEffect);
    return m_pEffect->dispatcher(m_pEffect, opcode, index, value, ptr, opt);
}

VstIntPtr VST2Plugin::HostCallback(VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float) const
{
    switch (opcode)
    {

    case audioMasterVersion: return kVstVersion;
    case audioMasterCurrentId: return m_pEffect->uniqueID;
    case audioMasterGetSampleRate: return m_audioFormat.samplesPerSec();
    case audioMasterGetBlockSize: return m_blockSize;
    case audioMasterGetCurrentProcessLevel: return kVstProcessLevelUnknown;
    case audioMasterGetAutomationState: return kVstAutomationOff;
    case audioMasterGetLanguage: return kVstLangEnglish;
    case audioMasterGetVendorVersion: return VSTConstants::VendorVersion();

    case audioMasterGetVendorString:
        strcpy_s(static_cast<char*>(ptr), kVstMaxVendorStrLen, VSTConstants::Vendor().c_str());
        return 1;

    case audioMasterGetProductString:
        strcpy_s(static_cast<char*>(ptr), kVstMaxProductStrLen, VSTConstants::Product().c_str());
        return 1;

    case audioMasterIdle:
        if (HasEditor()) Dispatcher(effEditIdle);
        break;

    case audioMasterSizeWindow:
        if (HasEditor())
        {
            auto rect = m_windowController->GetWindowRectangle();

            rect.right = rect.left + static_cast<int>(index);
            rect.bottom = rect.top + static_cast<int>(value);
            m_windowController->ResizeWindow(rect);
        }
        break;

    case audioMasterCanDo:
        for each (auto capability in Capabilities())
        {
            if (strcmp(capability.c_str(), static_cast<const char*>(ptr)) == 0)
                return 1;
        }
        break;

    default:
        break;
    }

    return 0;
}

void VST2Plugin::AllocateBuffrers()
{
    assert(m_pEffect);

    if (m_audioFormat == INVALID_AUDIO_FORMAT)
        throw exception("VST2Plugin: invalid audio format");

    if (m_pEffect->numInputs < m_audioFormat.channels() || m_pEffect->numOutputs < m_audioFormat.channels())
        throw exception("VST2Plugin: plugin doesn't support required numbers of inputs/outputs");

    m_inputBuffer.resize(m_pEffect->numInputs * m_blockSize);
    m_outputBuffer.resize(m_pEffect->numOutputs * m_blockSize);

    for (auto i = 0; i < m_pEffect->numInputs; i++)
        m_inputBufferPointers.push_back(&m_inputBuffer[i * m_blockSize]);

    for (int i = 0; i < m_pEffect->numOutputs; i++)
        m_outputBufferPointers.push_back(&m_outputBuffer[i * m_blockSize]);
}

void VST2Plugin::Initialize()
{
    m_logger.trace << "VST2Plugin: initializing '" << m_modulePath << "' plugin" << endl;

    if (m_module) throw exception("VST2Plugin: plugin is already initialized");

    m_module = make_unique<VST2Module>();
    m_pEffect = m_module->LoadModule(m_modulePath, StaticCallback);

    if (m_pEffect == nullptr || m_pEffect->magic != kEffectMagic)
        throw exception("VST2Plugin: invalid VST2 plugin");

    /* if (!HasFlag(effFlagsIsSynth))
         throw exception("VST2Plugin: plugin has no 'IsSynth' flag");*/

    m_pEffect->user = this;

    AllocateBuffrers();

    Dispatcher(effOpen);
    Dispatcher(effSetSampleRate, 0, 0, nullptr, static_cast<float>(m_audioFormat.samplesPerSec()));
    Dispatcher(effSetBlockSize, 0, m_blockSize);
    Dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
    Dispatcher(effMainsChanged, 0, 1);
    Dispatcher(effStartProcess);

    if (HasEditor())
    {
        auto window = m_windowController->GetWindow();

        if (window == nullptr) throw exception("VST2Plugin: cannot get editor window");

        Dispatcher(effEditOpen, 0, 0, window);

        ERect* erct = nullptr;
        Dispatcher(effEditGetRect, 0, 0, &erct);

        Rect rect = { erct->left, erct->top, erct->right, erct->bottom };
        m_windowController->ResizeWindow(rect);
    }
}

void VST2Plugin::Close()
{
    if (m_pEffect)
    {
        if (HasEditor()) Dispatcher(effEditClose);

        Dispatcher(effStopProcess);
        Dispatcher(effMainsChanged, 0, 0);
        Dispatcher(effClose);

        m_pEffect = nullptr;
    }

    m_module.reset();
}

size_t VST2Plugin::ProcessInterlaved(float* pInBuffer, float* pOutBuffer, size_t inputFrames)
{
    auto channels = m_audioFormat.channels();
    auto maxOutputFrames = m_outputBuffer.size() / channels;
    auto framesToProcess = min<size_t>(inputFrames, maxOutputFrames);

    for (size_t frame = 0; frame < framesToProcess; frame++)
    {
        auto offset = frame * channels;
        for (size_t channel = 0; channel < channels; channel++)
            memcpy(m_inputBufferPointers[channel] + frame, pInBuffer + offset + channel, sizeof(float));
    }

    m_pEffect->processReplacing(m_pEffect, m_inputBufferPointers.data(), m_outputBufferPointers.data(), VstInt32(framesToProcess));

    for (size_t frame = 0; frame < framesToProcess; frame++)
    {
        auto offset = frame * channels;
        for (size_t channel = 0; channel < channels; channel++)
            memcpy(pOutBuffer + offset + channel, m_outputBufferPointers[channel] + frame, sizeof(float));
    }

    return framesToProcess;
}
