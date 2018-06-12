#pragma once

#include <string>
#include <atlcomcli.h>

namespace common
{
    namespace win32
    {
        class HRException : public std::exception
        {
        public:
            explicit HRException(const char* pFile, const char* pLine);
            void operator =(HRESULT hr);

            std::string ErrorMessage() const;
            HRESULT Error() const;

        private:
            HRESULT m_hr;

            const char* m_pFile;
            const char* m_pLine;
        };

        // ReSharper disable IdentifierTypo

#define STRINGIFY2(m) #m
#define MEXPAND(m) m
#define STRINGIFY(m) STRINGIFY2(m)

typedef HRException hr_exception;
#define _hr hr_exception(MEXPAND(__FILE__), STRINGIFY(__LINE__))

    }
}
