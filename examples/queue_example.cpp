#include <iostream>
#include <thread>

#include "memory_engine/exception.hpp"
#include "memory_engine/thread_safe_queue.hpp"

int main()
{
    std::cout << "[queue_example] starting\n";
    std::cout << "  input: ThreadSafeQueue<int>(capacity=4), producer pushes 1..5, consumer drains until shutdown\n";

    memory_engine::ThreadSafeQueue<int> queue(4);

    std::thread producer([&queue]
                         {
        for (int value = 1; value <= 5; ++value)
        {
            std::cout << "  producer push: " << value << '\n';
            queue.push(value);
        }

        std::cout << "  producer: shutdown()\n";
        queue.shutdown(); });

    std::thread consumer([&queue]
                         {
        try
        {
            while (true)
            {
                const int value = queue.wait_and_pop();
                std::cout << "  consumer pop: " << value << '\n';
            }
        }
        catch (const memory_engine::QueueClosedException &)
        {
            std::cout << "  consumer: queue closed\n";
        } });

    producer.join();
    consumer.join();

    std::cout << "queue_example passed\n";
    return 0;
}
