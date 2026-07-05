# Benchmarking and Measurement Notes

Implementation files:

- include/memory_engine/timer.hpp
- include/memory_engine/statistics.hpp
- include/memory_engine/benchmark.hpp
- include/memory_engine/reporter.hpp
- benchmark/allocation_benchmark.cpp

## Components

## Timer

Timer uses std::chrono::steady_clock to avoid wall-clock discontinuities.

## BenchmarkStatistics

Tracks:

- iterations
- total_nanoseconds
- min_nanoseconds
- max_nanoseconds

Average is computed as total_nanoseconds / iterations.

## BenchmarkRunner

run(name, iterations, function):

1. Loops iterations times.
2. Creates timer per iteration.
3. Invokes callable.
4. Records elapsed nanoseconds.
5. Returns BenchmarkResult.

## BenchmarkReporter

Formats output as:

name: iterations=..., avg_ns=..., min_ns=..., max_ns=...

## Measurement Guidance

- Use Release builds for representative numbers.
- Run multiple repetitions and compare median, not only one run.
- Keep machine load stable while measuring.
- Report compiler version and CPU info with benchmarks.

## Limitations

- Current benchmark target is intentionally minimal and does not yet include full scenario matrix.
- Nanosecond granularity does not guarantee nanosecond precision on all platforms.
