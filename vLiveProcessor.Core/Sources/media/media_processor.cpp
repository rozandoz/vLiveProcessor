#include "media_processor.h"

using namespace std;
using namespace chrono;

MediaProcessor::MediaProcessor()
    : m_logger(Logger::GetInstance())
{
}

MediaProcessor::~MediaProcessor()
{
}

void MediaProcessor::Initialize(const ProcessorSettings& settings)
{
    try
    {
        m_settings = settings;

        if (bufferSamples() == 0)
            throw exception("Invalid buffer samples");
     
        OnInitialize();

        if (m_audioFormat == INVALID_AUDIO_FORMAT)
            throw exception("AudioFormat was not specified");

        if (m_consumer != nullptr) 
            m_consumer->SetAudioFormat(m_audioFormat);

       
    }
    catch (exception e)
    {
        m_logger.error << "Error initialize media processor. Reason: " << e.what();
        throw;
    }
}

void MediaProcessor::SetAudioFormat(const AudioFormat& audioFormat)
{
    try
    {
        OnValidateFormat(audioFormat);
        m_audioFormat = audioFormat;
    }
    catch (exception e)
    {
        m_logger.error << "Error set audio format. Reason: " << e.what();
        throw;
    }
}

bool MediaProcessor::AddBlock(milliseconds timeout, shared_ptr<MediaBlock> block)
{
    try
    {
        return OnAddBlock(timeout, block);
    }
    catch (exception e)
    {
        m_logger.error << "Error push block. Reason: " << e.what();
        throw;
    }
}

void MediaProcessor::SetConsumer(const shared_ptr<IConsumer>& consumer)
{
    m_consumer = consumer;
}

void MediaProcessor::Start()
{
    try
    {
        return OnStart();
    }
    catch (exception e)
    {
        m_logger.error << "Error start processor. Reason: " << e.what();
        throw;
    }
}

void MediaProcessor::Stop()
{
    try
    {
        return OnStop();
    }
    catch (exception e)
    {
        m_logger.error << "Error stop processor. Reason: " << e.what();
        throw;
    }
}

void MediaProcessor::Reset()
{
    try
    {
        return OnReset();
    }
    catch (exception e)
    {
        m_logger.error << "Error reset processor. Reason: " << e.what();
        throw;
    }
}
