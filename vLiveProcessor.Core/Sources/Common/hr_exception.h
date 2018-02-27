#pragma once

#include <string>
#include <winerror.h>

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
