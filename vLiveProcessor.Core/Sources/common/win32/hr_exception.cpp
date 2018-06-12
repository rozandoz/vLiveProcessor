#include "hr_exception.h"

#include "common/strings.h"
#include "strings_converter.h"

#include <comdef.h>

common::win32::HRException::HRException(const char* pFile, const char* pLine)
    : m_hr(S_OK)
    , m_pFile(pFile)
    , m_pLine(pLine)
{
}

void common::win32::HRException::operator=(HRESULT hr)
{
    m_hr = hr;
    if (FAILED(m_hr)) throw *this;
}

std::string common::win32::HRException::ErrorMessage() const
{
    auto error = _com_error(m_hr);
    auto errorMessage = WideToMultiByte(error.ErrorMessage());
    return StringFormat("%s (line: %s) - %s (0x%02x)", m_pFile, m_pLine, errorMessage, m_hr);
}

HRESULT common::win32::HRException::Error() const
{
    return m_hr;
}
