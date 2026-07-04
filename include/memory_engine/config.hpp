#pragma once

#include <cstddef>

namespace memory_engine
{
    inline constexpr std::size_t DEFAULT_POOL_SIZE = 1024;

    struct PoolStatistics
    {
        std::size_t current_usage = 0;
        std::size_t peak_usage = 0;
        std::size_t acquire_count = 0;
        std::size_t release_count = 0;
        std::size_t failed_acquires = 0;
    };
} // namespace memory_engine
