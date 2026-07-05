# ObjectPoolV2 Engineering Notes

Implementation file: include/memory_engine/object_pool_v2.hpp

## Why V2 Exists

V2 removes the eager-construction constraint of V1 and supports perfect-forwarded construction arguments per acquisition.

## Memory Layout

- One aligned raw block allocated with operator new(size, align_val_t).
- Slot i address = memory\_ + i \* sizeof(T).
- constructed\_[i] tracks whether slot i currently holds a live T.
- free*indices* stores reusable slot indices.

## Acquire Lifecycle

1. If no free slot exists, increment failed_acquires and return null handle.
2. Pop a slot index.
3. Call std::construct_at(ptr, args...).
4. On constructor throw: push index back, rethrow.
5. Mark slot constructed and update PoolStatistics.

## Release Lifecycle

1. Validate pointer and index assumptions via asserts.
2. Call std::destroy_at(ptr).
3. Mark slot unconstructed.
4. Push slot index back to free*indices*.
5. Update release_count and current_usage.

## Destruction Path

Pool destructor iterates over all slots and destroys only currently constructed objects, then frees aligned raw memory.

## Key Invariants

- constructed*[i] == 1 implies slot i is not in free_indices*.
- constructed\_[i] == 0 implies slot i is either free now or ready for reconstruction.
- current_usage <= peak_usage <= capacity.
- acquire_count - release_count == current_usage when no external corruption occurs.

## Exception Safety

- Strong guarantee for failed construction in acquire: pool state remains valid and slot is returned to free list.

## Complexity

- acquire/release bookkeeping: O(1)
- user constructor/destructor cost: dependent on T

## Trade-offs

- More flexible than V1, but has more state and stronger correctness requirements.
- Still not thread-safe by design.

## Recommended Use

Use when T is expensive to default-construct, requires constructor arguments, or when explicit lifetime control is needed.
