#pragma once

#include <cstddef>
#include <functional>
#include <string_view>
#include <utility>

#include "memory_engine/statistics.hpp"
#include "memory_engine/timer.hpp"

namespace memory_engine
{
    struct BenchmarkResult
    {
        std::string_view name;
        BenchmarkStatistics statistics;
    };

    class BenchmarkRunner
    {
    public:
        template <typename Func>
        [[nodiscard]] BenchmarkResult run(std::string_view name, std::size_t iterations, Func &&function) const
        {
            BenchmarkStatistics statistics;

            for (std::size_t index = 0; index < iterations; ++index)
            {
                Timer timer;
                std::invoke(std::forward<Func>(function));
                statistics.add_sample(timer.elapsed_nanoseconds());
            }

            return BenchmarkResult{name, statistics};
        }
    };
} // namespace memory_engine
