#include "ring_buffer.h"

#include <algorithm>

using namespace std;
using namespace chrono;

shared_ptr<common::memory::RingBuffer> common::memory::RingBuffer::Create(size_t capacity)
{
    return make_shared<RingBuffer>(capacity);
}

common::memory::RingBuffer::RingBuffer(size_t capacity)
{
    m_buffer.resize(capacity);

    m_map.push_back(Area{ 0, 0 });
    m_map.push_back(Area{ capacity, capacity });
}

void common::memory::RingBuffer::ReleaseArea(const Area& area)
{
    lock_guard<mutex> lock(m_critSec);
    m_map.erase(remove(m_map.begin(), m_map.end(), area), m_map.end());
}

bool common::memory::RingBuffer::TryGetBuffer(nanoseconds timeout, size_t size, shared_ptr<Buffer>& buffer)
{
    auto start = high_resolution_clock::now();

    do
    {
        {
            lock_guard<mutex> lock(m_critSec);

            auto min = m_buffer.size();
            auto li = 0, ri = 0;

            for (auto i = 0; i < m_map.size() - 1; i++)
            {
                auto la = m_map[i];
                auto ra = m_map[i + 1];

                auto dt = ra.begin - la.end;
                if (dt >= size && dt <= min)
                {
                    li = i; ri = i + 1;
                    min = dt;
                }
            }

            if (li != 0 || ri != 0)
            {
                Area area;
                area.begin = m_map[li].end;
                area.end = area.begin + size;
                m_map.insert(m_map.begin() + ri, area);

                auto data = m_buffer.data() + area.begin;
                buffer = make_shared<SharedBuffer>(shared_from_this(), data, size, area);

                return true;
            }
        }

        this_thread::sleep_for(1ms);

    } while (high_resolution_clock::now() - start < timeout);

    return false;
}

common::memory::RingBuffer::SharedBuffer::SharedBuffer(shared_ptr<RingBuffer> owner, char* pData, size_t size, const Area& area)
    : m_owner(owner)
    , m_area(area)
    , m_pData(pData)
    , m_maxSize(size)
    , m_size(0)
{
}

common::memory::RingBuffer::SharedBuffer::~SharedBuffer()
{
    m_owner->ReleaseArea(m_area);
}

char* common::memory::RingBuffer::SharedBuffer::data() const
{
    return m_pData;
}

size_t common::memory::RingBuffer::SharedBuffer::max_size() const
{
    return m_maxSize;
}

size_t common::memory::RingBuffer::SharedBuffer::size() const
{
    return m_size;
}

void common::memory::RingBuffer::SharedBuffer::set_size(size_t size)
{
    m_size = size;
}
