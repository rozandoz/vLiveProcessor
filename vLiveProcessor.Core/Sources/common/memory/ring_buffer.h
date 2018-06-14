#pragma once

#include <memory>
#include <chrono>
#include <mutex>
#include <vector>

#include "buffer.h"


namespace common
{
    namespace memory
    {
        class RingBuffer : public std::enable_shared_from_this<RingBuffer>
        {
            struct Area;
            class SharedBuffer;

            friend class std::_Ref_count_obj<RingBuffer>;

        public:
            static std::shared_ptr<RingBuffer> Create(size_t capacity);

        private:
            RingBuffer(size_t capacity);
            void ReleaseArea(const Area& area);

        public:
            bool TryGetBuffer(std::chrono::nanoseconds timeout, size_t size, std::shared_ptr<Buffer>& buffer);

        private:
            std::mutex              m_critSec;
            std::vector<char>       m_buffer;
            std::vector<Area>       m_map;

        private:
            struct Area
            {
                size_t begin;
                size_t end;

                friend bool operator==(const Area& lhs, const Area& rhs)
                {
                    return lhs.begin == rhs.begin
                        && lhs.end == rhs.end;
                }

                friend bool operator!=(const Area& lhs, const Area& rhs)
                {
                    return !(lhs == rhs);
                }
            };

            class SharedBuffer : public Buffer
            {
            public:
                SharedBuffer(std::shared_ptr<RingBuffer> owner, char* pData, size_t size, const Area& area);
                virtual ~SharedBuffer();

                char* data() const override;
                size_t max_size() const override;
                size_t size() const override;
                void set_size(size_t size) override;

            private:
                std::shared_ptr<RingBuffer>     m_owner;
                Area                            m_area;

                char                            *m_pData;
                size_t                          m_maxSize;
                size_t                          m_size;
            };

        };
    }
}



