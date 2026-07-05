# Testing and Validation Strategy

## Test Targets

- tests/object_pool_test.cpp
- tests/object_pool_v2_test.cpp
- tests/thread_safe_queue_test.cpp
- tests/benchmark_test.cpp

All are built as standalone executables and registered in CTest.

## Coverage Summary

## ObjectPool V1

- acquire/release behavior
- slot reuse
- exhaustion returns null
- compile-time validation for deleted copy/move

## ObjectPoolV2

- constructor argument forwarding
- slot reuse and statistics updates
- alignment behavior
- exhaustion and failed_acquires tracking
- exception safety when constructor throws
- destructor/lifetime tracking

## ThreadSafeQueue

- basic push/pop and size/empty state
- wait_and_pop behavior
- shutdown behavior and exception path
- multi-producer/multi-consumer scenario
- zero-capacity constructor rejection

## Benchmark Stack

- statistics accumulation and average/min/max correctness
- benchmark runner invocation count
- reporter output content

## How to Run

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## Reliability Notes

- Queue tests rely on synchronization and should be deterministic with current design.
- For race-sensitive future changes, add stress tests with larger iteration counts.
