#pragma once

#include <chrono>
#include <cstddef>

namespace memory_engine
{
    class Timer
    {
    public:
        using clock = std::chrono::steady_clock;
        using duration = clock::duration;

        Timer()
            : start_(clock::now())
        {
        }

        void reset() noexcept
        {
            start_ = clock::now();
        }

        [[nodiscard]] duration elapsed() const noexcept
        {
            return clock::now() - start_;
        }

        [[nodiscard]] std::size_t elapsed_nanoseconds() const noexcept
        {
            return static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed()).count());
        }

        [[nodiscard]] std::size_t elapsed_microseconds() const noexcept
        {
            return static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count());
        }

        [[nodiscard]] std::size_t elapsed_milliseconds() const noexcept
        {
            return static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count());
        }

    private:
        clock::time_point start_;
    };
} // namespace memory_engine
