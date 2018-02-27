#include "hr_exception.h"
#include "strings.h"

#include <comdef.h>

hr_exception::hr_exception(const wchar_t* pFile, const wchar_t* pLine)
    : m_hr(S_OK)
    , m_file(pFile)
    , m_line(pLine)
{
}

void hr_exception::operator=(HRESULT hr)
{
    m_hr = hr;
    if (FAILED(m_hr)) throw *this;
}

std::wstring hr_exception::ErrorMessage() const
{
    return wstring_format(L"%s (line: %s) - %s (0x%02x)", m_file.c_str(), m_line.c_str(), _com_error(m_hr).ErrorMessage(), m_hr);
}

HRESULT hr_exception::Error() const
{
    return m_hr;
}
