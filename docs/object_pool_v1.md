# ObjectPool V1 Engineering Notes

Implementation file: include/memory_engine/object_pool.hpp

## Design

ObjectPool<T> stores a fixed number of pre-constructed T objects in std::vector<T> and tracks free slots by index.

## Data Structures

- std::vector<T> objects\_: backing storage.
- std::stack<std::size*t> free_indices*: free-list of reusable slots.
- unique_ptr<T, Deleter>: returned handle type.

## Acquire and Release Path

1. acquire checks free*indices*.
2. If empty, returns unique_ptr(nullptr, Deleter{this}).
3. Otherwise pops an index and returns pointer to objects\_[index].
4. Deleter::operator() calls release(ptr).
5. release computes index by pointer arithmetic and pushes index back.

## Invariants

- free*indices* contains only valid indices in [0, capacity).
- A slot is either checked out exactly once or present in free*indices*.
- release must only be called for pointers originating from this pool.

## Safety Model

- Non-copyable and non-movable class avoids stale deleter back-pointers.
- assert guards catch misuse in debug builds.

## Complexity

- acquire: O(1)
- release: O(1)
- capacity/available/size/empty: O(1)

## Limitations

- Requires default-constructible T because vector is resized eagerly.
- Construction cost is paid up front for all slots.
- No thread safety.
- Misuse via foreign pointer to release is undefined behavior in release builds.

## Recommended Use

Use for lightweight types where eager construction and simple fixed-capacity reuse are acceptable.
