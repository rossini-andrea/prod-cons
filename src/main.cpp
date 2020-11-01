#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>

#include "fifo.hpp"

static prodcons::fifo<int> fifo;
static std::condition_variable stop_processing;
static std::mutex stop_processing_mutex;
static bool stopped;

void producer(void);
void consumer(void);

/**
 * @brief Application entry point.
 */
int main(void) {
    stopped = false;
    std::thread producer_task(producer);
    std::thread consumer_task(consumer);

    std::cin.get();

    {
        std::lock_guard<std::mutex> lock(stop_processing_mutex);
        stopped = true;
    }

    stop_processing.notify_all();
    producer_task.join();
    consumer_task.join();
}

/**
 * @brief Generates numbers ciclically.
 */
void producer() {
    int i = 0;
    std::unique_lock<std::mutex> processing_lock(stop_processing_mutex);

    while (!stop_processing.wait_for(processing_lock, std::chrono::milliseconds(0), []{ return stopped; })) {
        ++i;
        fifo.put(i);
        i = i % 100;
    }
}

/**
 * @brief Prints sum of first 10 items.
 */
void print_sum(void) {
    int result = 0;

    for (int i = 0; i < 10; ++i) {
        result += fifo.get();
    }

    std::cout << result << '\n';
}

/**
 * @brief Prints all data in the queue.
 */
void print_remaining_data(void) {
    std::cout << "Data left in queue:\n";

    while (fifo.count()) {
        std::cout << fifo.get() << '\n';
    }
}

/**
 * @brief Sums generated data.
 */
void consumer() {
    std::unique_lock<std::mutex> processing_lock(stop_processing_mutex);

    while (!stop_processing.wait_for(processing_lock, std::chrono::milliseconds(10), []{ return stopped; })) {
        while (fifo.count() >= 10) {
            print_sum();
        }
    }

    while (fifo.count() >= 10) {
        print_sum();
    }

    print_remaining_data();
}
