// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Common/errors.h"

int main()
{
    try
    {
        _hr = E_FAIL;
    }
    catch (hr_exception hr)
    {
        auto s = hr.Error();
    }

    return 0;
}

