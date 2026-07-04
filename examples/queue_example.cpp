#include <iostream>
#include <thread>

#include "memory_engine/exception.hpp"
#include "memory_engine/thread_safe_queue.hpp"

int main()
{
    memory_engine::ThreadSafeQueue<int> queue(4);

    std::thread producer([&queue]
                         {
        for (int value = 1; value <= 5; ++value)
        {
            queue.push(value);
        }

        queue.shutdown(); });

    std::thread consumer([&queue]
                         {
        try
        {
            while (true)
            {
                const int value = queue.wait_and_pop();
                std::cout << value << '\n';
            }
        }
        catch (const memory_engine::QueueClosedException &)
        {
        } });

    producer.join();
    consumer.join();

    return 0;
}
