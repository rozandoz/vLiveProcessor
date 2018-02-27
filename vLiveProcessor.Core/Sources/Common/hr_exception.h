#pragma once

#include <string>
#include <winerror.h>

class hr_exception : public std::exception
{
public:
    explicit hr_exception(const wchar_t* pFile, const wchar_t* pLine);
    void operator =(HRESULT hr);

    std::wstring ErrorMessage() const;
    HRESULT Error() const;

private:
    HRESULT         m_hr;
    std::wstring    m_file;
    std::wstring    m_line;
};
