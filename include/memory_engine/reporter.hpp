#pragma once

#include <iosfwd>
#include <ostream>

#include "memory_engine/benchmark.hpp"

namespace memory_engine
{
    class BenchmarkReporter
    {
    public:
        explicit BenchmarkReporter(std::ostream &output)
            : output_(output)
        {
        }

        void report(const BenchmarkResult &result) const
        {
            output_ << result.name << ": " << result.statistics << '\n';
        }

    private:
        std::ostream &output_;
    };
} // namespace memory_engine
