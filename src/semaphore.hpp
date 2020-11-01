#pragma once

#ifndef __SEMAPHORE_HPP
#define __SEMAPHORE_HPP

#include <thread>
#include <mutex>
#include <functional>

namespace prodcons {
/**
 * @brief A simple semaphore.
 */
template<size_t MaxCount>
class semaphore {
    size_t m_count;
    std::mutex m_access_mutex;
public:
    semaphore(size_t initial_count):
        m_count(initial_count) {

    }

    /**
     * @brief Returns the current count.
     */
    size_t count() {
        return m_count;
    }

    /**
     * @brief Release waiting thread and executes transaction.
     */
    void release(std::function<void(void)> transaction) {
        std::lock_guard<std::mutex> lock(m_access_mutex);

        while (m_count >= MaxCount) {
            m_access_mutex.unlock();
            std::this_thread::yield();
            m_access_mutex.lock();
        }

        transaction();
        ++m_count;
    }

    /**
     * @brief Waits for semaphore and executes transaction.
     */
    void aquire(std::function<void(void)> transaction) {
        std::lock_guard<std::mutex> lock(m_access_mutex);

        while (m_count <= 0) {
            m_access_mutex.unlock();
            std::this_thread::yield();
            m_access_mutex.lock();
        }

        transaction();
        --m_count;
    }
};
};
#endif
