#pragma once

#include <memory>

#include "common/logging/logger.h"
#include "media/interfaces/i_processor.h"
#include "media/interfaces/i_producer.h"
#include "media/interfaces/i_consumer.h"

class MediaProcessor : public IProcessor, public IProducer, public IConsumer
{
public:
    MediaProcessor();
    virtual ~MediaProcessor();

private:
    void Initialize(const ProcessorSettings& settings) override final;

    void SetAudioFormat(const AudioFormat& audioFormat) override final;
    bool AddBlock(std::chrono::nanoseconds timeout, std::shared_ptr<MediaBlock> block) override final;

    void SetConsumer(const std::shared_ptr<IConsumer>& consumer) override final;

    void Start() override final;
    void Stop() override final;
    void Reset() override final;

protected:
    virtual void OnInitialize() { }
    virtual void OnValidateFormat(const AudioFormat& format) { }
    virtual bool OnAddBlock(std::chrono::nanoseconds timeout, std::shared_ptr<MediaBlock> block) { return false; }
    virtual void OnStart() { }
    virtual void OnStop() { }
    virtual void OnReset() { }

    size_t bufferSamples() const { return m_settings.bufferSamples; }
    size_t buffersCount() const { return m_settings.buffersCount; }

protected:
    Logger&                         m_logger;
    AudioFormat                     m_audioFormat;
    ProcessorSettings               m_settings;

    std::shared_ptr<IConsumer>      m_consumer;

};


