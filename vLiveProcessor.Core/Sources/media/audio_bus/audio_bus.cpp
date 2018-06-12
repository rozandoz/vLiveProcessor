#include "audio_bus.h"

#include "media/interfaces/i_consumer.h"
#include "media/interfaces/i_producer.h"

using namespace std;

AudioBus::AudioBus()
    : m_logger(Logger::GetInstance())
    , m_started(false)

{
}

AudioBus::~AudioBus()
{
    Stop();
}

void AudioBus::SetProcessors(ProcessorsCollection& processors)
{
    lock_guard<mutex> lock(m_critSec);

    if (m_started) throw exception("Audio bus is started");

    m_processors.clear();
    m_processors.resize(processors.size());

    copy(processors.begin(), processors.end(), m_processors.begin());

    for (size_t i = 1; i <= processors.size() - 1; i++)
    {
        auto ls = processors[i - 1];
        auto rs = processors[i];

        auto producer = dynamic_pointer_cast<IProducer, IProcessor>(ls);
        auto consumer = dynamic_pointer_cast<IConsumer, IProcessor>(rs);

        if (producer == nullptr && consumer == nullptr)
            throw exception("Cannot mape processors");

        producer->SetConsumer(consumer);
    }

    for (auto processor : m_processors)
    {
        processor->Initialize(ProcessorSettings());
    }
}

void AudioBus::Start()
{
    lock_guard<mutex> lock(m_critSec);

    if (m_started) throw exception("Audio bus is already started");

    m_logger.trace << "AudioBus::Start" << endl;

    for (auto i = m_processors.rbegin(); i != m_processors.rend(); ++i)
    {
        i->get()->Start();
    }

    m_started = true;
}

void AudioBus::Stop()
{
    lock_guard<mutex> lock(m_critSec);

    if (!m_started) return;

    m_logger.trace << "AudioBus::Stop" << endl;

    for (auto& processor : m_processors)
    {
        processor->Stop();
    }

    m_started = false;
}
