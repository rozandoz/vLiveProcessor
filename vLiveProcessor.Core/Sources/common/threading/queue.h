#pragma once

#include <mutex>
#include <queue>

namespace common
{
    namespace threading
    {
        template<class T>
        class Queue
        {
        public:
            Queue();
            explicit Queue(size_t capacity);

            void SetCapacity(size_t capacity);
            bool TryAdd(std::chrono::nanoseconds timeout, const T& item);
            bool TryGet(std::chrono::nanoseconds timeout, T& item);

        private:
            std::queue<T>   m_queue;
            std::mutex      m_mutex;

            size_t          m_capacity;
        };

        template <class T>
        Queue<T>::Queue()
            : Queue<T>(-1)
        {
        }

        template <class T>
        Queue<T>::Queue(size_t capacity)
        {
            SetCapacity(capacity);
        }

        template <class T>
        void Queue<T>::SetCapacity(size_t capacity)
        {
            capacity = capacity == -1 ? SIZE_MAX : capacity;

            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_queue.size() > capacity)
                throw std::runtime_error("queue is not empty");

            m_capacity = capacity;
        }

        template <class T>
        bool Queue<T>::TryAdd(std::chrono::nanoseconds timeout, const T& item)
        {
            auto start = std::chrono::high_resolution_clock::now();

            do
            {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (m_queue.size() < m_capacity)
                    {
                        m_queue.push(item);
                        return true;
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            } while (std::chrono::high_resolution_clock::now() - start < timeout);

            return false;
        }

        template <class T>
        bool Queue<T>::TryGet(std::chrono::nanoseconds timeout, T& item)
        {
            auto start = std::chrono::high_resolution_clock::now();

            do
            {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    if (!m_queue.empty())
                    {
                        item = m_queue.front();
                        m_queue.pop();
                        return true;
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            } while (std::chrono::high_resolution_clock::now() - start < timeout);

            return false;
        }
    }
}
