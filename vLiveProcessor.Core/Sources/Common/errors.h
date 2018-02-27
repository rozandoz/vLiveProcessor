#pragma once

#include "hr_exception.h"

//HRESULT codes
#define E_DEVICE_STARTED                    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_AUDIO, 0x200+101)
#define E_DEVICE_NOT_STARTED                MAKE_HRESULT(SEVERITY_ERROR, FACILITY_AUDIO, 0x200+102)
//-------------


#define STRINGIFY2(m) #m
#define MEXPAND(m) m
#define STRINGIFY(m) STRINGIFY2(m)
#define WIDE(m) _T(m)

#define _hr hr_exception(WIDE(MEXPAND(__FILE__)), WIDE(STRINGIFY(__LINE__)))
