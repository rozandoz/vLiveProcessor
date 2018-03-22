#pragma once

#include <string>
#include <atlcomcli.h>

class HRException : public std::exception
{
public:
    explicit HRException(const wchar_t* pFile, const wchar_t* pLine);
    void operator =(HRESULT hr);

    std::wstring ErrorMessage() const;
    HRESULT Error() const;

private:
    HRESULT         m_hr;
    std::wstring    m_file;
    std::wstring    m_line;
};

// ReSharper disable IdentifierTypo

#define STRINGIFY2(m) #m
#define MEXPAND(m) m
#define STRINGIFY(m) STRINGIFY2(m)
#define WIDE(m) _T(m)

typedef HRException hr_exception;
#define _hr hr_exception(WIDE(MEXPAND(__FILE__)), WIDE(STRINGIFY(__LINE__)))
