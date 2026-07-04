#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "memory_engine/object_pool_v2.hpp"

namespace
{
    struct Widget
    {
        int left = 0;
        int right = 0;

        Widget(int l, int r)
            : left(l), right(r)
        {
        }

        [[nodiscard]] int sum() const noexcept
        {
            return left + right;
        }
    };

    struct Tracking
    {
        static inline int alive = 0;
        static inline int destroyed = 0;

        explicit Tracking(int value)
            : value(value)
        {
            ++alive;
        }

        ~Tracking()
        {
            ++destroyed;
            --alive;
        }

        int value = 0;
    };

    struct ThrowingWidget
    {
        explicit ThrowingWidget(int value)
        {
            if (value == 42)
            {
                throw std::runtime_error("boom");
            }

            payload = value;
        }

        int payload = 0;
    };

    struct alignas(64) AlignedWidget
    {
        explicit AlignedWidget(int value)
            : payload(value)
        {
        }

        int payload = 0;
    };

    void reset_tracking()
    {
        Tracking::alive = 0;
        Tracking::destroyed = 0;
    }

    void test_forwarding_reuse_and_statistics()
    {
        reset_tracking();

        std::size_t first_address = 0;

        {
            memory_engine::ObjectPoolV2<Widget> pool(1);

            assert(pool.capacity() == 1);
            assert(pool.available() == 1);
            assert(pool.size() == 0);
            assert(pool.empty());

            auto first = pool.acquire(3, 4);
            assert(first);
            assert(first->sum() == 7);
            assert(pool.available() == 0);
            assert(pool.size() == 1);
            assert(!pool.empty());

            const auto &stats_after_acquire = pool.statistics();
            assert(stats_after_acquire.acquire_count == 1);
            assert(stats_after_acquire.release_count == 0);
            assert(stats_after_acquire.failed_acquires == 0);
            assert(stats_after_acquire.current_usage == 1);
            assert(stats_after_acquire.peak_usage == 1);

            first_address = reinterpret_cast<std::size_t>(first.get());
            first.reset();

            assert(pool.available() == 1);
            assert(pool.size() == 0);
            assert(pool.empty());
            assert(pool.statistics().release_count == 1);
            assert(pool.statistics().current_usage == 0);

            auto second = pool.acquire(9, 1);
            assert(second);
            assert(reinterpret_cast<std::size_t>(second.get()) == first_address);
            assert(second->sum() == 10);
        }
    }

    void test_exhaustion_and_exception_safety()
    {
        {
            memory_engine::ObjectPoolV2<AlignedWidget> pool(1);
            auto item = pool.acquire(7);
            assert(item);
            auto address = reinterpret_cast<std::uintptr_t>(item.get());
            assert(address % alignof(AlignedWidget) == 0);

            auto exhausted = pool.acquire(8);
            assert(!exhausted);
            assert(pool.statistics().failed_acquires == 1);
        }

        {
            memory_engine::ObjectPoolV2<ThrowingWidget> pool(1);

            assert(pool.available() == 1);
            try
            {
                auto invalid = pool.acquire(42);
                (void)invalid;
                assert(false && "Expected construction to throw");
            }
            catch (const std::runtime_error &)
            {
            }

            assert(pool.available() == 1);
            assert(pool.size() == 0);
            assert(pool.statistics().current_usage == 0);

            auto valid = pool.acquire(7);
            assert(valid);
            assert(valid->payload == 7);
        }
    }

    void test_lifetime_tracking()
    {
        reset_tracking();

        {
            memory_engine::ObjectPoolV2<Tracking> pool(2);
            {
                auto first = pool.acquire(1);
                auto second = pool.acquire(2);
                assert(first->value == 1);
                assert(second->value == 2);
                assert(Tracking::alive == 2);
            }

            assert(Tracking::alive == 0);
            assert(Tracking::destroyed == 2);
        }

        assert(Tracking::alive == 0);
    }
}

int main()
{
    test_forwarding_reuse_and_statistics();
    test_exhaustion_and_exception_safety();
    test_lifetime_tracking();

    std::cout << "object_pool_v2_test passed\n";
    return 0;
}
