#include <atomic>
#include <cassert>
#include <cstddef>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "memory_engine/exception.hpp"
#include "memory_engine/thread_safe_queue.hpp"

namespace
{
    void print_case_header(const char *name)
    {
        std::cout << "[thread_safe_queue_test] " << name << '\n';
    }

    void test_basic_push_try_pop_and_size()
    {
        print_case_header("test_basic_push_try_pop_and_size");
        std::cout << "  input: ThreadSafeQueue<int>(capacity=2), push(1), push(2), try_pop twice\n";
        memory_engine::ThreadSafeQueue<int> queue(2);

        assert(queue.empty());
        assert(queue.size() == 0);

        queue.push(1);
        queue.push(2);
        std::cout << "  output: queue size after pushes=" << queue.size() << '\n';

        assert(!queue.empty());
        assert(queue.size() == 2);

        int value = 0;
        assert(queue.try_pop(value));
        assert(value == 1);
        std::cout << "  output: first try_pop returned value=" << value << '\n';

        assert(queue.try_pop(value));
        assert(value == 2);
        std::cout << "  output: second try_pop returned value=" << value << '\n';

        assert(!queue.try_pop(value));
        assert(queue.empty());
        assert(queue.size() == 0);
        std::cout << "  output: queue empty after pops=" << std::boolalpha << queue.empty() << '\n';
    }

    void test_wait_and_pop_and_shutdown()
    {
        print_case_header("test_wait_and_pop_and_shutdown");
        std::cout << "  input: ThreadSafeQueue<int>(capacity=1), consumer waits, producer pushes(7), shutdown()\n";
        memory_engine::ThreadSafeQueue<int> queue(1);

        std::promise<int> result_promise;
        auto result_future = result_promise.get_future();

        std::thread consumer([&queue, promise = std::move(result_promise)]() mutable
                             { promise.set_value(queue.wait_and_pop()); });

        queue.push(7);
        const int popped = result_future.get();
        assert(popped == 7);
        std::cout << "  output: wait_and_pop returned " << popped << '\n';

        queue.shutdown();
        consumer.join();

        bool threw = false;
        try
        {
            queue.push(8);
        }
        catch (const memory_engine::QueueClosedException &)
        {
            threw = true;
        }

        assert(threw);
        std::cout << "  output: push after shutdown threw QueueClosedException\n";
    }

    void test_multi_producer_multi_consumer()
    {
        print_case_header("test_multi_producer_multi_consumer");
        std::cout << "  input: 4 producers x 250 integers, 4 consumers, queue capacity=16\n";
        constexpr std::size_t producer_count = 4;
        constexpr std::size_t consumer_count = 4;
        constexpr std::size_t per_producer = 250;
        constexpr std::size_t total_items = producer_count * per_producer;

        memory_engine::ThreadSafeQueue<std::size_t> queue(16);
        std::atomic<std::size_t> consumed{0};
        std::atomic<std::size_t> sum{0};
        std::vector<std::thread> consumers;
        std::vector<std::thread> producers;

        consumers.reserve(consumer_count);
        for (std::size_t index = 0; index < consumer_count; ++index)
        {
            consumers.emplace_back([&queue, &consumed, &sum]
                                   {
                try
                {
                    while (true)
                    {
                        const std::size_t value = queue.wait_and_pop();
                        consumed.fetch_add(1, std::memory_order_relaxed);
                        sum.fetch_add(value, std::memory_order_relaxed);
                    }
                }
                catch (const memory_engine::QueueClosedException &)
                {
                } });
        }

        producers.reserve(producer_count);
        for (std::size_t producer_index = 0; producer_index < producer_count; ++producer_index)
        {
            producers.emplace_back([producer_index, &queue]
                                   {
                const std::size_t start = producer_index * per_producer + 1;
                const std::size_t end = start + per_producer;

                for (std::size_t value = start; value < end; ++value)
                {
                    queue.push(value);
                } });
        }

        for (auto &producer : producers)
        {
            producer.join();
        }

        queue.shutdown();

        for (auto &consumer : consumers)
        {
            consumer.join();
        }

        const std::size_t expected_sum = total_items * (total_items + 1) / 2;
        assert(consumed.load(std::memory_order_relaxed) == total_items);
        assert(sum.load(std::memory_order_relaxed) == expected_sum);
        assert(queue.empty());
        std::cout << "  output: consumed=" << consumed.load(std::memory_order_relaxed)
                  << ", sum=" << sum.load(std::memory_order_relaxed)
                  << ", expected_sum=" << expected_sum << '\n';
    }

    void test_zero_capacity_rejected()
    {
        print_case_header("test_zero_capacity_rejected");
        std::cout << "  input: ThreadSafeQueue<int>(capacity=0)\n";
        bool threw = false;
        try
        {
            memory_engine::ThreadSafeQueue<int> queue(0);
            (void)queue;
        }
        catch (const std::invalid_argument &)
        {
            threw = true;
        }

        assert(threw);
        std::cout << "  output: constructor rejected zero capacity with std::invalid_argument\n";
    }
}

int main()
{
    std::cout << "[thread_safe_queue_test] starting\n";
    test_basic_push_try_pop_and_size();
    test_wait_and_pop_and_shutdown();
    test_multi_producer_multi_consumer();
    test_zero_capacity_rejected();

    std::cout << "thread_safe_queue_test passed\n";
    return 0;
}
