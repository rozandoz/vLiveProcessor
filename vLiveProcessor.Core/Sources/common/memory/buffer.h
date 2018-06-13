#pragma once

namespace common
{
    namespace memory
    {
        class Buffer
        {
        protected:
            virtual ~Buffer() {}

        public:
            virtual char* data() const = 0;
            virtual size_t max_size() const = 0;
            virtual size_t size() const = 0;
            virtual void set_size(size_t size) = 0;
        };
    }
}
