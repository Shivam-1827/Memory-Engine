#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <stack>
#include <vector>

namespace memory_engine
{

    template <typename T>
    class ObjectPool
    {
    private:
        struct Deleter
        {
            ObjectPool *pool = nullptr;

            void operator()(T *ptr) const;
        };

    public:
        using value_type = T;
        using pointer = std::unique_ptr<T, Deleter>;

        explicit ObjectPool(std::size_t capacity);

        ObjectPool(const ObjectPool &) = delete;
        ObjectPool &operator=(const ObjectPool &) = delete;
        ObjectPool(ObjectPool &&) = delete;
        ObjectPool &operator=(ObjectPool &&) = delete;

        pointer acquire();

        std::size_t available() const noexcept;

        std::size_t capacity() const noexcept;

        std::size_t size() const noexcept;

        bool empty() const noexcept;

    private:
        void release(T *ptr);

    private:
        std::vector<T> objects_;
        std::stack<std::size_t> free_indices_;
    };

    template <typename T>
    ObjectPool<T>::ObjectPool(std::size_t capacity)
    {
        objects_.resize(capacity);

        for (std::size_t i = capacity; i > 0; --i)
        {
            free_indices_.push(i - 1);
        }
    }

    template <typename T>
    std::size_t ObjectPool<T>::capacity() const noexcept
    {
        return objects_.size();
    }

    template <typename T>
    std::size_t ObjectPool<T>::available() const noexcept
    {
        return free_indices_.size();
    }

    template <typename T>
    std::size_t ObjectPool<T>::size() const noexcept
    {
        return capacity() - available();
    }

    template <typename T>
    bool ObjectPool<T>::empty() const noexcept
    {
        return free_indices_.empty();
    }

    template <typename T>
    void ObjectPool<T>::release(T *ptr)
    {
        assert(ptr != nullptr);

        const std::size_t index = ptr - objects_.data();

        free_indices_.push(index);
    }

    template <typename T>
    void ObjectPool<T>::Deleter::operator()(T *ptr) const
    {
        if (pool && ptr)
        {
            pool->release(ptr);
        }
    }

    template <typename T>
    typename ObjectPool<T>::pointer
    ObjectPool<T>::acquire()
    {
        if (free_indices_.empty())
        {
            return pointer(nullptr, Deleter{this});
        }

        std::size_t index = free_indices_.top();
        free_indices_.pop();

        return pointer(
            &objects_[index],
            Deleter{this});
    }

} // namespace memory_engine