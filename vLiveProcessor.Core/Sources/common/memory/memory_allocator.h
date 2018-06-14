#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>

#include "buffer.h"

namespace common
{
    namespace memory
    {
        class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator>
        {
            friend class std::_Ref_count_obj<MemoryAllocator>;

        public:
            class MemoryBuffer;

        private:
            typedef std::vector<char>                    DataBuffer;
            typedef std::shared_ptr<DataBuffer>          SharedDataBuffer;
            typedef std::shared_ptr<MemoryAllocator>     SharedAllocator;

            MemoryAllocator(size_t bufferSize, size_t buffersCount);
            ~MemoryAllocator();

            void ReturnBuffer(SharedDataBuffer sharedBuffer);

        public:
            static SharedAllocator Create(size_t bufferSize, size_t buffersCount);
            bool TryGetBuffer(std::chrono::nanoseconds timeout, std::shared_ptr<Buffer>& buffer);

        private:
            std::mutex                          m_critSec;
            std::queue<SharedDataBuffer>        m_buffers;

        public:
            class MemoryBuffer : public Buffer
            {
                friend class std::_Ref_count_obj<MemoryBuffer>;

                MemoryBuffer(SharedAllocator owner, SharedDataBuffer sharedBuffer);
                virtual ~MemoryBuffer();

            public:
                char* data() const override;
                size_t max_size() const override;
                size_t size() const override;
                void set_size(size_t size) override;

            private:
                size_t                  m_size;

                SharedAllocator         m_owner;
                SharedDataBuffer        m_buffer;
            };
        };
    }
}



