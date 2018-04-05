// vLiveProcessor.Core.Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "common/logging/logger.h"
#include "common/media/audio_format.h"

using namespace std;
using namespace common::media;

int main()
{
    auto& logger = Logger::GetInstance();

    AudioFormat format(2, 32, 48000);
    logger.trace << format;

    getchar();

    return 0;
}
