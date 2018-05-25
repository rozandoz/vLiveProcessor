#include "vst2_plugin.h"

#include <cassert>

#ifdef WIN32
#include "common/win32/strings_converter.h"
#endif // WIN32

using namespace std;
using namespace common::win32;

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

VST2Plugin::VST2Plugin(const VST2PluginSettings& settings, std::shared_ptr<VST2Host> host)
    : m_settings(settings)
    , m_pEffect(nullptr)
    , m_logger(Logger::GetInstance())
{
    Initialize();
}

VST2Plugin::~VST2Plugin()
{
    Close();
}

//Temp

void VST2Plugin::ResizeEditor(const RECT & clientRc) const
{
    if (m_settings.window)
    {
        auto rc = clientRc;
        const auto style = GetWindowLongPtr((HWND)m_settings.window, GWL_STYLE);
        const auto exStyle = GetWindowLongPtr((HWND)m_settings.window, GWL_EXSTYLE);
        const BOOL fMenu = GetMenu((HWND)m_settings.window) != nullptr;
        AdjustWindowRectEx(&rc, style, fMenu, exStyle);
        MoveWindow((HWND)m_settings.window, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }
}

bool VST2Plugin::HasFlag(int32_t flag) const
{
    assert(m_pEffect);
    return (m_pEffect->flags & flag) == flag;
}

bool VST2Plugin::HasEditor() const
{
    auto hasEditor = HasFlag(effFlagsHasEditor);

#ifdef WIN32
    return hasEditor && m_settings.window;
#else // WIN32
    return false;
#endif
}

intptr_t VST2Plugin::Dispatcher(int32_t opcode, int32_t index, intptr_t value, void * ptr, float opt) const
{
    assert(m_pEffect);
    return m_pEffect->dispatcher(m_pEffect, opcode, index, value, ptr, opt);
}

void VST2Plugin::AllocateBuffrers()
{
    assert(m_pEffect);

    auto blockSize = m_settings.blockSize;

    m_inputBuffer.resize(m_pEffect->numInputs * blockSize);
    m_outputBuffer.resize(m_pEffect->numOutputs * blockSize);

    for (auto i = 0; i < m_pEffect->numInputs; i++)
        m_inputBufferPointers.push_back(&m_inputBuffer[i * blockSize]);

    for (int i = 0; i < m_pEffect->numOutputs; i++)
        m_outputBufferPointers.push_back(&m_outputBuffer[i * blockSize]);
}

void VST2Plugin::Initialize()
{
    m_logger.trace << "VST2Plugin: initializing '" << m_settings.modulePath << "' plugin" << endl;

    if (m_module) throw exception("VST2Plugin: plugin is already initialized");

    m_module = make_unique<VST2Module>();
    m_pEffect = m_module->LoadModule(m_settings.modulePath, StaticCallback);

    if (m_pEffect == nullptr || m_pEffect->magic != kEffectMagic)
        throw exception("VST2Plugin: invalid VST2 plugin");

    if (!HasFlag(effFlagsIsSynth))
        throw exception("VST2Plugin: plugin has no 'IsSynth' flag");

    m_pEffect->user = this;

    AllocateBuffrers();

    Dispatcher(effOpen);
    Dispatcher(effSetSampleRate, 0, 0, 0, static_cast<float>(m_settings.sampleRate));
    Dispatcher(effSetBlockSize, 0, m_settings.blockSize);
    Dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
    Dispatcher(effMainsChanged, 0, 1);
    Dispatcher(effStartProcess);

    if (HasFlag(effFlagsHasEditor))
    {
        WNDCLASSEX wcex{ sizeof(wcex) };
        wcex.lpfnWndProc = DefWindowProc;
        wcex.hInstance = GetModuleHandle(0);
        wcex.lpszClassName = L"Minimal VST host - Guest VST Window Frame";
        RegisterClassEx(&wcex);

        const auto style = WS_CAPTION | WS_THICKFRAME | WS_OVERLAPPEDWINDOW;
        m_settings.window = CreateWindow(wcex.lpszClassName, L"Window", style, 0, 0, 0, 0, GetConsoleWindow(), 0, 0, 0);

        Dispatcher(effEditOpen, 0, 0, m_settings.window);
        RECT rc{};
        ERect* erc = nullptr;
        Dispatcher(effEditGetRect, 0, 0, &erc);
        rc.left = erc->left;
        rc.top = erc->top;
        rc.right = erc->right;
        rc.bottom = erc->bottom;

        ResizeEditor(rc);
        ShowWindow((HWND)m_settings.window, SW_SHOW);
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

