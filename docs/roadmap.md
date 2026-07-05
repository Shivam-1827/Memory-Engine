# Engineering Roadmap

This roadmap focuses on incremental, testable improvements.

## Near-Term

- Add benchmark scenarios comparing heap allocation, ObjectPool V1, and ObjectPoolV2 under identical workloads.
- Add queue throughput and latency benchmark targets.
- Add API-level documentation comments for public interfaces.

## Mid-Term

- Add optional thread-safe wrapper for ObjectPoolV2 with explicit contention strategy.
- Add optional timed queue operations (wait_for push/pop).
- Add configurable benchmark output formats (CSV/JSON).

## Long-Term

- Explore arena/slab allocators.
- Explore lock-free queue variant with strict correctness tests.
- Add CI matrix for multiple compilers and sanitizer builds.

## Non-Goals (Current Scope)

- Replacing system allocator globally.
- Providing a production lock-free API without extensive verification.
