#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <ostream>
#include <string_view>

namespace memory_engine
{
    struct BenchmarkStatistics
    {
        std::size_t iterations = 0;
        std::size_t total_nanoseconds = 0;
        std::size_t min_nanoseconds = std::numeric_limits<std::size_t>::max();
        std::size_t max_nanoseconds = 0;

        void add_sample(std::size_t nanoseconds) noexcept
        {
            ++iterations;
            total_nanoseconds += nanoseconds;
            min_nanoseconds = std::min(min_nanoseconds, nanoseconds);
            max_nanoseconds = std::max(max_nanoseconds, nanoseconds);
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return iterations == 0;
        }

        [[nodiscard]] double average_nanoseconds() const noexcept
        {
            if (iterations == 0)
            {
                return 0.0;
            }

            return static_cast<double>(total_nanoseconds) / static_cast<double>(iterations);
        }
    };

    inline std::ostream &operator<<(std::ostream &output, const BenchmarkStatistics &statistics)
    {
        output << "iterations=" << statistics.iterations
               << ", avg_ns=" << statistics.average_nanoseconds()
               << ", min_ns=" << (statistics.empty() ? 0 : statistics.min_nanoseconds)
               << ", max_ns=" << statistics.max_nanoseconds;
        return output;
    }
} // namespace memory_engine
