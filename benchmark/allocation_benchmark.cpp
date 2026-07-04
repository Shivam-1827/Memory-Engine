#include <iostream>

#include "memory_engine/benchmark.hpp"
#include "memory_engine/reporter.hpp"

int main()
{
    memory_engine::BenchmarkRunner runner;
    memory_engine::BenchmarkReporter reporter(std::cout);

    auto result = runner.run("empty benchmark", 10, [] {});

    reporter.report(result);
    return 0;
}
