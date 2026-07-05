# ThreadSafeQueue Engineering Notes

Implementation file: include/memory_engine/thread_safe_queue.hpp

## Model

ThreadSafeQueue<T> is a bounded blocking queue with explicit shutdown.

## Synchronization Primitives

- std::mutex mutex\_: protects all shared state.
- std::condition*variable not_empty*: coordinates consumers.
- std::condition*variable not_full*: coordinates producers.

## State

- std::queue<T> queue\_: FIFO data.
- size*type capacity*: max queue size, fixed at construction.
- bool shutdown\_: termination flag.

## Producer Path (push)

1. Acquire unique_lock.
2. Wait on not*full* until shutdown* or queue*.size() < capacity\_.
3. If shutdown\_ is true, throw QueueClosedException.
4. Push value and notify one waiting consumer.

## Consumer Paths

- try_pop: non-blocking pop under lock; returns false if empty.
- wait*and_pop: waits until shutdown* or non-empty; if empty after wake, throws QueueClosedException.

## Shutdown Semantics

- shutdown() sets shutdown\_ = true and notifies all waiters.
- Waiting producers/consumers unblock.
- Further push calls fail with QueueClosedException.

## Invariants

- queue*.size() <= capacity*.
- All state transitions involving queue* and shutdown* occur while mutex\_ is held.
- Wake predicates are re-checked after every condition_variable wake.

## Correctness Notes

- Uses predicate-based waits to handle spurious wakeups safely.
- notify_one is used for normal push/pop progress; notify_all is used for shutdown broadcast.

## Complexity

- push: O(1) amortized + blocking time.
- try_pop: O(1) amortized.
- wait_and_pop: O(1) amortized + blocking time.

## Failure Modes

- Capacity 0 throws invalid_argument at construction.
- Push after shutdown throws QueueClosedException.
