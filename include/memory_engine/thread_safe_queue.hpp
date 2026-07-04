#pragma once

#include <cstddef>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <utility>

#include "memory_engine/exception.hpp"

namespace memory_engine
{
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        using value_type = T;
        using size_type = std::size_t;

        explicit ThreadSafeQueue(size_type capacity)
            : capacity_(capacity)
        {
            if (capacity_ == 0)
            {
                throw std::invalid_argument("ThreadSafeQueue capacity must be greater than zero");
            }
        }

        ThreadSafeQueue(const ThreadSafeQueue &) = delete;
        ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;
        ThreadSafeQueue(ThreadSafeQueue &&) = delete;
        ThreadSafeQueue &operator=(ThreadSafeQueue &&) = delete;

        void push(T value)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            not_full_.wait(lock, [this]
                           { return shutdown_ || queue_.size() < capacity_; });

            if (shutdown_)
            {
                throw QueueClosedException();
            }

            queue_.push(std::move(value));
            not_empty_.notify_one();
        }

        [[nodiscard]] bool try_pop(T &value)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty())
            {
                return false;
            }

            value = std::move(queue_.front());
            queue_.pop();
            not_full_.notify_one();
            return true;
        }

        T wait_and_pop()
        {
            std::unique_lock<std::mutex> lock(mutex_);
            not_empty_.wait(lock, [this]
                            { return shutdown_ || !queue_.empty(); });

            if (queue_.empty())
            {
                throw QueueClosedException();
            }

            T value = std::move(queue_.front());
            queue_.pop();
            not_full_.notify_one();
            return value;
        }

        [[nodiscard]] bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

        [[nodiscard]] size_type size() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }

        void shutdown()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            shutdown_ = true;
            not_full_.notify_all();
            not_empty_.notify_all();
        }

    private:
        mutable std::mutex mutex_;
        std::condition_variable not_empty_;
        std::condition_variable not_full_;
        std::queue<T> queue_;
        size_type capacity_ = 0;
        bool shutdown_ = false;
    };
} // namespace memory_engine
