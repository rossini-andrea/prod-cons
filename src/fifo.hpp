#pragma once

#ifndef __FIFO_HPP
#define __FIFO_HPP

#include "semaphore.hpp"
#include <mutex>
#include <array>

namespace prodcons {

/**
 * @brief A simple FIFO queue.
 */
template<typename T, size_t Capacity = 1024>
class fifo {
    size_t m_put_index, m_get_index;
    std::array<T, Capacity> m_buffer;
    semaphore<Capacity> m_count_semaphore;

public:
    fifo() : m_count_semaphore(0),
            m_put_index(0),
            m_get_index(0) {

    }

    /**
     * @brief Return the current item count.
     */
    size_t count() {
        return m_count_semaphore.count();
    }

    /**
     * @brief Queues a new item.
     */
    void put(T item) {
        m_count_semaphore.release([&](){
            m_buffer[m_put_index] = item;
            m_put_index = (m_put_index + 1) % Capacity;
        });
    }

    /**
     * @brief Reads the first available item.
     */
    T get() {
        T item;
        m_count_semaphore.aquire([&](){
            item = m_buffer[m_get_index];
            m_get_index = (m_get_index + 1) % Capacity;
        });

        return item;
    }
};
};
#endif
