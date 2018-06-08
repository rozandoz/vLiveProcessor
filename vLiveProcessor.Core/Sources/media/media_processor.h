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
    void Initialize() override final;

    void SetAudioFormat(const AudioFormat& audioFormat) override final;
    bool AddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) override final;

    void SetConsumer(const std::shared_ptr<IConsumer>& consumer) override final;

    void Start() override final;
    void Stop() override final;
    void Reset() override final;

protected:
    virtual void OnInitialize() { }
    virtual void OnValidateFormat(const AudioFormat& format) { }
    virtual bool OnAddBlock(uint32_t timeout, std::shared_ptr<MediaBlock> block) { return false; }
    virtual void OnStart() { }
    virtual void OnStop() { }
    virtual void OnReset() { }

protected:
    Logger&                         m_logger;
    AudioFormat                     m_audioFormat;

    std::shared_ptr<IConsumer>      m_consumer;

};


