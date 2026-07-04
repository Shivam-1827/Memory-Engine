#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

#include "memory_engine/config.hpp"

namespace memory_engine
{

    template <typename T>
    class ObjectPoolV2
    {
    private:
        struct Deleter
        {
            ObjectPoolV2 *pool = nullptr;

            void operator()(T *ptr) const noexcept;
        };

    public:
        using value_type = T;
        using unique_pointer = std::unique_ptr<T, Deleter>;
        using pointer = unique_pointer;

        explicit ObjectPoolV2(std::size_t capacity);
        ~ObjectPoolV2();

        ObjectPoolV2(const ObjectPoolV2 &) = delete;
        ObjectPoolV2 &operator=(const ObjectPoolV2 &) = delete;
        ObjectPoolV2(ObjectPoolV2 &&) = delete;
        ObjectPoolV2 &operator=(ObjectPoolV2 &&) = delete;

        template <typename... Args>
        [[nodiscard]] unique_pointer acquire(Args &&...args);

        [[nodiscard]]
        std::size_t available() const noexcept;

        [[nodiscard]]
        std::size_t capacity() const noexcept;

        [[nodiscard]]
        std::size_t size() const noexcept;

        [[nodiscard]]
        bool empty() const noexcept;

        [[nodiscard]]
        const PoolStatistics &statistics() const noexcept;

    private:
        void release(T *ptr);
        T *element_ptr(std::size_t index) noexcept;
        const T *element_ptr(std::size_t index) const noexcept;

    private:
        std::byte *memory_ = nullptr;
        std::vector<std::uint8_t> constructed_;
        std::vector<std::size_t> free_indices_;
        PoolStatistics stats_;
        std::size_t capacity_ = 0;
    };

    template <typename T>
    ObjectPoolV2<T>::ObjectPoolV2(std::size_t capacity)
        : memory_(nullptr), constructed_(capacity, 0), free_indices_(), stats_(), capacity_(capacity)
    {
        if (capacity_ == 0)
        {
            return;
        }

        free_indices_.reserve(capacity_);

        memory_ = static_cast<std::byte *>(::operator new(
            capacity_ * sizeof(T),
            std::align_val_t(alignof(T))));

        for (std::size_t i = capacity_; i > 0; --i)
        {
            free_indices_.push_back(i - 1);
        }
    }

    template <typename T>
    ObjectPoolV2<T>::~ObjectPoolV2()
    {
        for (std::size_t index = 0; index < capacity_; ++index)
        {
            if (constructed_[index] != 0)
            {
                element_ptr(index)->~T();
            }
        }

        if (memory_)
        {
            ::operator delete(memory_, std::align_val_t(alignof(T)));
        }
    }

    template <typename T>
    std::size_t ObjectPoolV2<T>::capacity() const noexcept
    {
        return capacity_;
    }

    template <typename T>
    std::size_t ObjectPoolV2<T>::available() const noexcept
    {
        return free_indices_.size();
    }

    template <typename T>
    std::size_t ObjectPoolV2<T>::size() const noexcept
    {
        return stats_.current_usage;
    }

    template <typename T>
    bool ObjectPoolV2<T>::empty() const noexcept
    {
        return stats_.current_usage == 0;
    }

    template <typename T>
    const PoolStatistics &ObjectPoolV2<T>::statistics() const noexcept
    {
        return stats_;
    }

    template <typename T>
    T *ObjectPoolV2<T>::element_ptr(std::size_t index) noexcept
    {
        assert(index < capacity_);
        return std::launder(reinterpret_cast<T *>(memory_ + (index * sizeof(T))));
    }

    template <typename T>
    const T *ObjectPoolV2<T>::element_ptr(std::size_t index) const noexcept
    {
        assert(index < capacity_);
        return std::launder(reinterpret_cast<const T *>(memory_ + (index * sizeof(T))));
    }

    template <typename T>
    void ObjectPoolV2<T>::release(T *ptr)
    {
        assert(ptr != nullptr);
        assert(memory_ != nullptr);
        assert(capacity_ > 0);

        const std::size_t index = static_cast<std::size_t>(ptr - element_ptr(0));
        assert(index < capacity_);
        assert(constructed_[index] != 0);

        std::destroy_at(ptr);
        constructed_[index] = 0;
        free_indices_.push_back(index);
        ++stats_.release_count;
        assert(stats_.current_usage > 0);
        --stats_.current_usage;
    }

    template <typename T>
    void ObjectPoolV2<T>::Deleter::operator()(T *ptr) const noexcept
    {
        if (pool && ptr)
        {
            pool->release(ptr);
        }
    }

    template <typename T>
    template <typename... Args>
    typename ObjectPoolV2<T>::unique_pointer ObjectPoolV2<T>::acquire(Args &&...args)
    {
        if (free_indices_.empty())
        {
            ++stats_.failed_acquires;
            return unique_pointer(nullptr, Deleter{this});
        }

        const std::size_t index = free_indices_.back();
        free_indices_.pop_back();

        T *ptr = element_ptr(index);
        try
        {
            std::construct_at(ptr, std::forward<Args>(args)...);
        }
        catch (...)
        {
            free_indices_.push_back(index);
            throw;
        }

        constructed_[index] = 1;
        ++stats_.acquire_count;
        ++stats_.current_usage;
        if (stats_.current_usage > stats_.peak_usage)
        {
            stats_.peak_usage = stats_.current_usage;
        }

        return unique_pointer(ptr, Deleter{this});
    }

} // namespace memory_engine
