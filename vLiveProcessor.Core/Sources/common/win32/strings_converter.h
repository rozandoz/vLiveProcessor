#pragma once

#include <string>

namespace common
{
    namespace win32
    {
        std::string WideToMultiByte(std::wstring wstr);
        std::wstring MultibyteToWide(std::string str);
    }
}
