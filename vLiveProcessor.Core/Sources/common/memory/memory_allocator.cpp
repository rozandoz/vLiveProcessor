#include "memory_allocator.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

common::memory::MemoryAllocator::MemoryAllocator(size_t bufferSize, size_t buffersCount)
{
    for (size_t i = 0; i < buffersCount; i++)
    {
        m_buffers.push(make_shared<DataBuffer>(bufferSize));
    }
}

common::memory::MemoryAllocator::~MemoryAllocator()
{
}

void common::memory::MemoryAllocator::ReturnBuffer(SharedDataBuffer sharedBuffer)
{
    lock_guard<mutex> lock(m_critSec);
    m_buffers.push(sharedBuffer);
}

shared_ptr<common::memory::MemoryAllocator> common::memory::MemoryAllocator::Create(size_t bufferSize, size_t buffersCount)
{
    return make_shared<MemoryAllocator>(bufferSize, buffersCount);
}

bool common::memory::MemoryAllocator::TryGetBuffer(nanoseconds timeout, shared_ptr<Buffer>& buffer)
{
    auto start = high_resolution_clock::now();

    do
    {
        lock_guard<mutex> lock(m_critSec);

        {
            if (m_buffers.size() != 0)
            {
                buffer = make_shared<MemoryBuffer>(shared_from_this(), m_buffers.front());
                m_buffers.pop();
                return true;
            }
        }

        this_thread::sleep_for(1ms);
        
    } while (duration_cast<nanoseconds>(high_resolution_clock::now() - start) < timeout);

    return false;
}

common::memory::MemoryAllocator::MemoryBuffer::MemoryBuffer(SharedAllocator owner, SharedDataBuffer sharedBuffer)
    : m_size(0)
    , m_owner(owner)
    , m_buffer(sharedBuffer)
{

}

common::memory::MemoryAllocator::MemoryBuffer::~MemoryBuffer()
{
    if (m_owner)
    {
        m_owner->ReturnBuffer(m_buffer);
    }
}

char* common::memory::MemoryAllocator::MemoryBuffer::data() const
{
    return m_buffer->data();
}

size_t common::memory::MemoryAllocator::MemoryBuffer::max_size() const
{
    return m_buffer->size();
}

size_t common::memory::MemoryAllocator::MemoryBuffer::size() const
{
    return m_size;
}

void common::memory::MemoryAllocator::MemoryBuffer::set_size(size_t size)
{
    m_size = size;
}
