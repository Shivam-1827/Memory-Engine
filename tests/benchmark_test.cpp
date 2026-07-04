#include <cassert>
#include <iostream>
#include <sstream>
#include <string_view>

#include "memory_engine/benchmark.hpp"
#include "memory_engine/reporter.hpp"
#include "memory_engine/timer.hpp"

namespace
{
    void test_timer_and_statistics()
    {
        memory_engine::Timer timer;
        timer.reset();

        memory_engine::BenchmarkStatistics statistics;
        statistics.add_sample(10);
        statistics.add_sample(20);
        statistics.add_sample(30);

        assert(statistics.iterations == 3);
        assert(statistics.total_nanoseconds == 60);
        assert(statistics.min_nanoseconds == 10);
        assert(statistics.max_nanoseconds == 30);
        assert(statistics.average_nanoseconds() == 20.0);
        (void)timer.elapsed_nanoseconds();
    }

    void test_benchmark_runner_and_reporter()
    {
        memory_engine::BenchmarkRunner runner;
        std::ostringstream output;
        memory_engine::BenchmarkReporter reporter(output);
        std::size_t counter = 0;

        const auto result = runner.run("noop", 5, [&counter]
                                       { ++counter; });

        assert(counter == 5);
        assert(result.name == "noop");
        assert(result.statistics.iterations == 5);
        assert(result.statistics.min_nanoseconds <= result.statistics.max_nanoseconds);

        reporter.report(result);
        const std::string text = output.str();
        assert(text.find("noop") != std::string::npos);
        assert(text.find("iterations=5") != std::string::npos);
    }
}

int main()
{
    test_timer_and_statistics();
    test_benchmark_runner_and_reporter();

    std::cout << "benchmark_test passed\n";
    return 0;
}
