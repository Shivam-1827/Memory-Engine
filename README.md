# Memory Engine

Memory Engine is a C++20 header-first systems library that implements:

- a fixed-capacity object pool (`ObjectPool<T>`)
- a raw-memory object pool with explicit lifetime control (`ObjectPoolV2<T>`)
- a bounded multi-producer/multi-consumer blocking queue (`ThreadSafeQueue<T>`)
- small benchmarking utilities (`Timer`, `BenchmarkStatistics`, `BenchmarkRunner`, `BenchmarkReporter`)

The repository also includes standalone examples, test executables, and a benchmark target built with CMake.

## Current Status

| Area                       | Status                                       |
| -------------------------- | -------------------------------------------- |
| ObjectPool V1              | Complete                                     |
| ObjectPool V2              | Complete                                     |
| ThreadSafeQueue            | Complete                                     |
| Benchmark utilities        | Complete                                     |
| Examples                   | Complete                                     |
| Tests                      | Complete (basic + behavioral coverage)       |
| Benchmark data publication | Pending (run and record numbers per machine) |

## Project Layout

```text
.
├── CMakeLists.txt
├── README.md
├── benchmark/
│   └── allocation_benchmark.cpp
├── docs/
│   ├── architecture.md
│   ├── benchmarking.md
│   ├── index.md
│   ├── object_pool_v1.md
│   ├── object_pool_v2.md
│   ├── roadmap.md
│   ├── testing_and_validation.md
│   └── thread_safe_queue.md
├── examples/
│   ├── object_pool_example.cpp
│   └── queue_example.cpp
├── include/
│   └── memory_engine/
│       ├── benchmark.hpp
│       ├── config.hpp
│       ├── exception.hpp
│       ├── object_pool.hpp
│       ├── object_pool_v2.hpp
│       ├── reporter.hpp
│       ├── statistics.hpp
│       ├── thread_safe_queue.hpp
│       └── timer.hpp
└── tests/
    ├── benchmark_test.cpp
    ├── object_pool_test.cpp
    ├── object_pool_v2_test.cpp
    └── thread_safe_queue_test.cpp
```

## Build Requirements

- CMake 3.16+
- C++20 compiler (GCC/Clang/MSVC with C++20 support)

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For debug builds:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## Executables Produced

Configured in `CMakeLists.txt`:

- `object_pool_example`
- `queue_example`
- `allocation_benchmark`
- `object_pool_test`
- `object_pool_v2_test`
- `thread_safe_queue_test`
- `benchmark_test`

## Run Examples

```bash
./build/object_pool_example
./build/queue_example
```

## Run Benchmark

```bash
./build/allocation_benchmark
```

## Benchmark Workflow

Use the following flow to collect reproducible benchmark results on your machine.

1. Build in Release mode.
2. Run benchmark multiple times.
3. Store raw output and report median/average.
4. Record compiler and CPU details with results.

```bash
# 1) Build optimized binaries
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# 2) Run benchmark repeatedly and save output
for i in 1 2 3 4 5; do
    echo "--- run $i ---" >> benchmark_runs.txt
    ./build/allocation_benchmark >> benchmark_runs.txt
done

# 3) (Optional) capture compiler and CPU context
c++ --version > benchmark_env.txt
uname -a >> benchmark_env.txt
lscpu >> benchmark_env.txt
```

Add your summarized values in the table below.

| Metric               | Value                | Notes                       |
| -------------------- | -------------------- | --------------------------- |
| Benchmark executable | allocation_benchmark | Current benchmark target    |
| Number of runs       | 5 (recommended)      | Increase if results vary    |
| Median time per run  | TBD                  | Fill from captured output   |
| Average time per run | TBD                  | Fill from captured output   |
| Min / Max            | TBD / TBD            | Useful for stability checks |
| Compiler             | TBD                  | Example: g++ 13.x           |
| Build type           | Release              | Use optimized build         |
| CPU / machine        | TBD                  | Include model information   |

## Run Tests

Using CTest:

```bash
ctest --test-dir build --output-on-failure
```

Or run binaries directly:

```bash
./build/object_pool_test
./build/object_pool_v2_test
./build/thread_safe_queue_test
./build/benchmark_test
```

## Design Notes

- `ObjectPool<T>` stores pre-constructed objects in `std::vector<T>` and reuses slots via a free-index stack.
- `ObjectPoolV2<T>` allocates raw aligned storage, constructs objects with `std::construct_at`, destroys with `std::destroy_at`, and tracks usage statistics.
- Both pools return `std::unique_ptr<T, Deleter>` handles so objects are returned to the pool automatically on scope exit.
- `ThreadSafeQueue<T>` uses `std::mutex` + `std::condition_variable` for bounded blocking semantics and supports graceful `shutdown()`.
- Queue shutdown causes waiting operations to unblock; pushes after shutdown throw `QueueClosedException`.

## Documentation

For a deeper walkthrough, see:

- `docs/index.md`
- `docs/architecture.md`
- `docs/object_pool_v1.md`
- `docs/object_pool_v2.md`
- `docs/thread_safe_queue.md`
- `docs/benchmarking.md`
- `docs/testing_and_validation.md`
- `docs/roadmap.md`

## Notes On Performance Results

Benchmark values depend on CPU, compiler, and optimization flags. Record and compare measurements on your own machine when making performance claims.
