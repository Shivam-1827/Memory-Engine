#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>

#include "memory_engine/object_pool.hpp"

namespace
{
    struct Tracking
    {
        static inline int alive = 0;
        static inline int destroyed = 0;

        Tracking()
        {
            ++alive;
        }

        ~Tracking()
        {
            ++destroyed;
            --alive;
        }
    };

    void reset_tracking()
    {
        Tracking::alive = 0;
        Tracking::destroyed = 0;
    }

    void test_basic_acquire_release_and_reuse()
    {
        reset_tracking();

        std::size_t first_address = 0;

        {
            memory_engine::ObjectPool<Tracking> pool(2);

            assert(pool.capacity() == 2);
            assert(pool.available() == 2);
            assert(pool.size() == 0);
            assert(!pool.empty());

            auto first = pool.acquire();
            assert(first);
            assert(pool.available() == 1);
            assert(pool.size() == 1);
            assert(!pool.empty());

            first_address = reinterpret_cast<std::size_t>(first.get());
            first.reset();

            assert(pool.available() == 2);
            assert(pool.size() == 0);
            assert(!pool.empty());

            auto second = pool.acquire();
            assert(second);
            assert(reinterpret_cast<std::size_t>(second.get()) == first_address);

            auto third = pool.acquire();
            assert(third);
            assert(pool.empty());
        }

        assert(Tracking::alive == 0);
        assert(Tracking::destroyed == 2);
    }

    void test_exhaustion_returns_null()
    {
        reset_tracking();

        memory_engine::ObjectPool<Tracking> pool(1);
        auto first = pool.acquire();
        auto second = pool.acquire();

        assert(first);
        assert(!second);
        assert(pool.available() == 0);
        assert(pool.size() == 1);
    }

    void test_copy_and_move_are_disabled()
    {
        static_assert(!std::is_copy_constructible_v<memory_engine::ObjectPool<Tracking>>);
        static_assert(!std::is_copy_assignable_v<memory_engine::ObjectPool<Tracking>>);
        static_assert(!std::is_move_constructible_v<memory_engine::ObjectPool<Tracking>>);
        static_assert(!std::is_move_assignable_v<memory_engine::ObjectPool<Tracking>>);
    }
}

int main()
{
    test_basic_acquire_release_and_reuse();
    test_exhaustion_returns_null();
    test_copy_and_move_are_disabled();

    std::cout << "object_pool_test passed\n";
    return 0;
}
