#include "media_processor_async.h"

MediaProcessorAsync::MediaProcessorAsync()
{
}

MediaProcessorAsync::~MediaProcessorAsync()
{
    MediaProcessorAsync::OnStop();
}

void MediaProcessorAsync::OnStart()
{
    StartThread();
}

void MediaProcessorAsync::OnStop()
{
    StopThread();
}

void MediaProcessorAsync::OnReset()
{
    OnStart();
    OnStop();
}


