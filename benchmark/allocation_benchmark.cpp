#include <iostream>

#include "memory_engine/benchmark.hpp"
#include "memory_engine/reporter.hpp"

int main()
{
    std::cout << "[allocation_benchmark] starting\n";
    std::cout << "  input: BenchmarkRunner::run(\"empty benchmark\", iterations=10, noop lambda)\n";

    memory_engine::BenchmarkRunner runner;
    memory_engine::BenchmarkReporter reporter(std::cout);

    auto result = runner.run("empty benchmark", 10, [] {});

    std::cout << "  output: benchmark name=\"" << result.name << "\"\n";
    std::cout << "  output: " << result.statistics << '\n';
    reporter.report(result);
    std::cout << "allocation_benchmark passed\n";
    return 0;
}
