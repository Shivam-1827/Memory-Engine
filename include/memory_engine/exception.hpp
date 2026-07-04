#pragma once

#include <stdexcept>
#include <string>

namespace memory_engine
{
    class PoolExhaustedException : public std::runtime_error
    {
    public:
        PoolExhaustedException()
            : std::runtime_error("Object pool exhausted")
        {
        }
    };

    class QueueClosedException : public std::runtime_error
    {
    public:
        QueueClosedException()
            : std::runtime_error("Thread-safe queue is closed")
        {
        }
    };
} // namespace memory_engine
