#pragma once

#include <string>  
#include <iostream>  

using namespace std;

using namespace System;
using namespace Runtime::InteropServices;

private ref class Strings
{
public:
    static string MarshalString(String^ s)
    {
        const char* ptr = nullptr;

        try
        {
            ptr = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
            return string(ptr);
        }
        finally
        {
            Marshal::FreeHGlobal(IntPtr((void*)ptr));
        }
    }
};


