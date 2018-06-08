#include "vst2_module.h"

#ifdef WIN32
#include "common/win32/strings_converter.h"
using namespace common::win32;
#endif // WIN32

using namespace std;

VST2Module::VST2Module()
{
#ifdef WIN32
    m_hModule = nullptr;
#endif // WIN32
}

VST2Module::~VST2Module()
{
#ifdef WIN32
    if (m_hModule) 
    {
        FreeLibrary(m_hModule);
        m_hModule = nullptr;
    }
#endif // WIN32
}

AEffect * VST2Module::LoadModule(std::string modulePath, audioMasterCallback callback)
{
#ifdef WIN32
    return LoadModule(MultibyteToWide(modulePath).c_str(), callback);
#else // WIN32
    throw exception("LoadModule method is not implemented");
#endif
}

AEffect * VST2Module::LoadModule(const wchar_t * pModulePath, audioMasterCallback callback)
{
    if (m_hModule != nullptr) throw exception("Module is already loaded");
    
    m_hModule = LoadLibrary(pModulePath);

    if (m_hModule == NULL)
    {
        auto errorCode = error_code(GetLastError(), system_category());
        throw system_error(errorCode, "Cannot load VST2 plugin module");
    }

    string entryNames[] =
    {
        "VSTPluginMain",
        "main"
    };

    for each (auto entryName in entryNames)
    {
        auto* vstEntryProc = reinterpret_cast<VstEntryProc*>(GetProcAddress(m_hModule, entryName.c_str()));
        if (vstEntryProc) return vstEntryProc(callback);
    }

    throw exception("Entry point of the VST2 plugin module was not found");
}
