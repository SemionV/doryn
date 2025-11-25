#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>
#include <dory/macros/assert.h>

namespace dory::containers
{
    template <typename T, typename TAllocator>
    class BasicList
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using allocator_type  = TAllocator;
        using reference       = T&;
        using const_reference = const T&;
        using pointer         = T*;
        using const_pointer   = const T*;

    private:
        allocator_type& _allocator;
        pointer _data   = nullptr;
        size_type _size = 0;
        size_type _capacity = 0;

    public:

    // ----------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------

        explicit BasicList(allocator_type& allocator) noexcept:
            _allocator(allocator)
        {}

        BasicList(const size_type count, const T& value, allocator_type& allocator):
            _allocator(allocator)
        {
            _data = static_cast<T*>(_allocator.allocate(count * sizeof(T)));
            _size = count;
            _capacity = count;

            for (size_type i = 0; i < count; ++i)
            {
                ::new (static_cast<void*>(&_data[i])) T(value);
            }
        }

        explicit BasicList(const size_type count, allocator_type& allocator):
            _allocator(allocator)
        {
            _data = static_cast<T*>(_allocator.allocate(count * sizeof(T)));
            _size = count;
            _capacity = count;
        }

        BasicList(const BasicList&) = delete;  // stateful allocators forbid copying

        BasicList(BasicList&& other) noexcept:
            _allocator(other._allocator),
            _data(other._data),
            _size(other._size),
            _capacity(other._capacity)
        {
            other._size = 0;
            other._capacity = 0;
            other._data = nullptr;
        }

        BasicList& operator=(BasicList&& other) noexcept
        {

            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;

            other._size = 0;
            other._capacity = 0;
            other._data = nullptr;

            return *this;
        }

        ~BasicList()
        {
            _allocator.deallocate(_data, _capacity * sizeof(T));
        }


    // ----------------------------------------------------------
    // Element access
    // ----------------------------------------------------------

        reference operator[](size_type index) noexcept
        {
            return _data[index];
        }
        const_reference operator[](size_type index) const noexcept
        {
            return _data[index];
        }

        reference at(size_type index)
        {
            assert::inhouse(index < _size, "Index is out of range");
            return _data[index];
        }
        const_reference at(size_type index) const
        {
            assert::inhouse(index < _size, "Index is out of range");
            return _data[index];
        }

        reference front() noexcept
        {
            assert::inhouse(_size > 0, "Get front element in an empty list");
            return _data[0];
        }

        const_reference front() const noexcept
        {
            assert::inhouse(_size > 0, "Get front element in an empty list");
            return _data[0];
        }

        reference back() noexcept
        {
            assert::inhouse(_size > 0, "Get back element in an empty list");
            return _data[_size - 1];
        }
        const_reference back() const noexcept
        {
            assert::inhouse(_size > 0, "Get back element in an empty list");
            return _data[_size - 1];
        }

        pointer data() noexcept
        {
            return _data;
        }

        const_pointer data() const noexcept
        {
            return _data;
        }

    // ----------------------------------------------------------
    // Standard Iterator support
    // ----------------------------------------------------------

        using iterator = T*;
        using const_iterator = const T*;

        iterator begin() noexcept { return _data; }
        iterator end() noexcept { return _data + _size; }

        const_iterator begin() const noexcept { return _data; }
        const_iterator end() const noexcept { return _data + _size; }

        const_iterator cbegin() const noexcept { return _data; }
        const_iterator cend() const noexcept { return _data + _size; }

    // ----------------------------------------------------------
    // Capacity
    // ----------------------------------------------------------

        [[nodiscard]] size_type size() const noexcept
        {
            return _size;
        }

        [[nodiscard]] size_type capacity() const noexcept
        {
            return _capacity;
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return _size == 0;
        }

        void reserve(const size_type newCap)
        {
            if (newCap > _capacity)
                reallocate(newCap);
        }

        void shrink_to_fit()
        {
            if (_size == 0)
            {
                // free everything
                if (_data)
                {
                    _allocator.deallocate(_data, _capacity * sizeof(T));
                }
                _data = nullptr;
                _capacity = 0;
                return;
            }

            if (_capacity > _size)
            {
                reallocate(_size); // shrink to tight fit
            }
        }


    // ----------------------------------------------------------
    // Modifiers
    // ----------------------------------------------------------

        void clear() noexcept
        {
            destroy_range(0, _size);
        }

        void push_back(const T& value)
        {
            if (_size == _capacity)
            {
                const size_type newCap = increaseCapacity(_capacity);
                reallocate(newCap);
            }

            construct_at(_size, value);

            ++_size;
        }

        void push_back(T&& value)
        {
            if (_size == _capacity)
            {
                const size_type newCap = increaseCapacity(_capacity);
                reallocate(newCap);
            }

            construct_at(_size, value);

            ++_size;
        }

        template<typename... Args>
        reference emplace_back(Args&&... args)
        {
            if (_size == _capacity)
            {
                const size_type newCap = increaseCapacity(_capacity);
                reallocate(newCap);
            }

            new (&_data[_size]) T(std::forward<Args>(args)...);
            return _data[_size++];
        }

        template<typename... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        {
            size_type index = pos - cbegin();  // convert iterator to index
            assert::inhouse(index <= _size, "Invalid insertion position");

            // Grow if needed
            if (_size == _capacity)
                reallocate(increaseCapacity(_capacity));

            // Shift elements to the right
            if (index < _size)
            {
                if constexpr (std::is_nothrow_move_constructible_v<T>)
                {
                    // Move elements backward (from end to index)
                    new (&_data[_size]) T(std::move(_data[_size - 1]));
                    for (size_type i = _size - 1; i > index; --i)
                    {
                        _data[i] = std::move(_data[i - 1]);
                    }
                }
                else
                {
                    // Copy fallback
                    new (&_data[_size]) T(_data[_size - 1]);
                    for (size_type i = _size - 1; i > index; --i)
                    {
                        _data[i] = _data[i - 1];
                    }
                }
            }

            // Construct element at position
            new (&_data[index]) T(std::forward<Args>(args)...);

            ++_size;
            return _data + index;
        }

        void pop_back()
        {
            assert::inhouse(_size > 0, "Pop back in an empty list");

            destroy_range(_size - 1, _size);
            --_size;
        }

        void resize(const size_type newSize, const T& defaultValue)
        {
            const size_type oldSize = _size;

            // Case 1: shrink
            if (newSize < oldSize)
            {
                destroy_range(newSize, oldSize);
                _size = newSize;
                return;
            }

            // Case 2: grow
            if (newSize > _capacity)
            {
                // capacity strategy can be improved, but this works:
                reallocate(newSize);
            }

            // Construct new objects
            for (size_type i = oldSize; i < newSize; ++i)
            {
                construct_at(i, defaultValue);   // value-initialize
            }

            _size = newSize;
        }

        void resize(size_type newSize)
        {
            resize(newSize, T{});
        }

        void assign(const size_type count, const T& value)
        {
            // Case 1: if count <= _size
            // We can overwrite existing elements and destroy the rest.
            if (count <= _size)
            {
                // Overwrite existing elements
                for (size_type i = 0; i < count; ++i)
                {
                    _data[i] = value;  // assign, not reconstruct
                }

                // Destroy the tail if shrinking
                destroy_range(count, _size);

                _size = count;
                return;
            }

            // Case 2: if count > _size and fits in capacity
            // Overwrite old + construct new ones.
            if (count <= _capacity)
            {
                // Overwrite existing elements
                for (size_type i = 0; i < _size; ++i)
                {
                    _data[i] = value;
                }

                // Construct new elements
                for (size_type i = _size; i < count; ++i)
                {
                    construct_at(i, value);
                }

                _size = count;
                return;
            }

            // Case 3: need to grow
            // Optimal to pick a growth policy (doubling), but count is required minimum.
            size_type newCap = count;
            reallocate(newCap);

            // Now construct all elements fresh
            for (size_type i = 0; i < count; ++i)
            {
                construct_at(i, value);
            }

            _size = count;
        }

        void swap(BasicList& other) noexcept
        {
            assert::inhouse(&_allocator == &other._allocator, "Swapping lists with different allocators is not allowed");

            using std::swap;

            swap(_data,     other._data);
            swap(_size,     other._size);
            swap(_capacity, other._capacity);
        }


    private:
    // ----------------------------------------------------------
    // Internal helpers
    // ----------------------------------------------------------

        static size_type increaseCapacity(const size_type currentCapacity)
        {
            return currentCapacity == 0 ? 1 : currentCapacity * 2;
        }

        // Performs raw memory reallocation + moves/copies elements
        void reallocate(const size_type newCap)
        {
            assert::inhouse(newCap >= _size, "Existing data will not fit into the new buffer");

            // Allocate new block
            T* newData = static_cast<T*>(_allocator.allocate(newCap * sizeof(T)));

            // Move or copy elements
            if constexpr (std::is_nothrow_move_constructible_v<T>)
            {
                // Best path: no-throw move
                for (size_type i = 0; i < _size; ++i)
                {
                    new (&newData[i]) T(std::move(_data[i]));
                }
            }
            else if constexpr (std::is_copy_constructible_v<T>)
            {
                // Fallback: copy if move potentially throws
                for (size_type i = 0; i < _size; ++i)
                {
                    new (&newData[i]) T(_data[i]);
                }
            }
            else
            {
                static_assert(std::is_move_constructible_v<T>, "BasicList<T> requires T to be move-constructible or copy-constructible.");
            }

            destroy_range(0, _size);

            // Deallocate old memory
            if (_data)
                _allocator.deallocate(_data, _capacity * sizeof(T));

            // Install new block
            _data = newData;
            _capacity = newCap;
        }

        void destroy_range(const size_type begin, const size_type end) noexcept
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                if (begin >= end)
                    return; // empty range, nothing to do

                assert::inhouse(_data != nullptr, "destroy_range called with null data pointer");

                for (size_type i = begin; i < end; ++i)
                    _data[i].~T();
            }
        }

        // Construction helpers
        void construct_at(size_type index, const T& value)
        {
            assert::inhouse(index < _capacity, "Index out of bounds");
            new (&_data[index]) T(value);
        }

        template<typename U>
        void construct_at(size_type index, U&& value)
        {
            assert::inhouse(index < _capacity, "Index out of bounds");

            if constexpr (std::is_trivially_constructible_v<T, U&&>)
            {
                // For trivial types we do not need placement new.
                _data[index] = static_cast<U&&>(value);
            }
            else
            {
                // Proper placement-new construction
                ::new (static_cast<void*>(&(_data[index]))) T(static_cast<U&&>(value));
            }
        }

        template<typename... Args>
        void construct_at_emplace(size_type index, Args&&... args)
        {
            assert::inhouse(index < _capacity, "Index out of bounds");

            ::new (static_cast<void*>(&_data[index]))
                T(std::forward<Args>(args)...);
        }
    };
}