#include <string>

#include <Windows.h>

#include "pluginterfaces/vst2.x/aeffectx.h"

typedef AEffect* (VstEntryProc)(audioMasterCallback);

class VST2Module 
{
public:
    VST2Module();
    ~VST2Module();

    AEffect* LoadModule(std::string modulePath, audioMasterCallback callback);

private:
#ifdef WIN32
    HMODULE     m_hModule;
    AEffect*    LoadModule(const wchar_t* pModulePath, audioMasterCallback callback);
#endif // WIN32
};