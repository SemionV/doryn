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

        BasicList(size_type count, const T& value, allocator_type& allocator);

        explicit BasicList(size_type count, allocator_type& allocator);

        BasicList(const BasicList&) = delete;  // stateful allocators forbid copying

        BasicList(BasicList&& other) noexcept;

        BasicList& operator=(BasicList&& other) noexcept;

        ~BasicList();


    // ----------------------------------------------------------
    // Element access
    // ----------------------------------------------------------

        reference operator[](size_type index) noexcept;
        const_reference operator[](size_type index) const noexcept;

        reference at(size_type index);
        const_reference at(size_type index) const;

        reference front() noexcept;
        const_reference front() const noexcept;

        reference back() noexcept;
        const_reference back() const noexcept;

        pointer data() noexcept;
        const_pointer data() const noexcept;


    // ----------------------------------------------------------
    // Capacity
    // ----------------------------------------------------------

        [[nodiscard]] size_type size() const noexcept;
        [[nodiscard]] size_type capacity() const noexcept;
        [[nodiscard]] bool empty() const noexcept;

        void reserve(size_type newCap);
        void shrink_to_fit();


    // ----------------------------------------------------------
    // Modifiers
    // ----------------------------------------------------------

        void clear() noexcept;

        void push_back(const T& value);
        void push_back(T&& value);

        template<typename... Args>
        reference emplace_back(Args&&... args);

        void pop_back();

        void resize(size_type newSize);
        void resize(size_type newSize, const T& defaultValue);

        void assign(size_type count, const T& value);

        void swap(BasicList& other) noexcept;


    private:
    // ----------------------------------------------------------
    // Internal helpers
    // ----------------------------------------------------------

        // Performs raw memory reallocation + moves/copies elements
        void reallocate(const size_type newCap)
        {
            assert::inhouse(newCap > _capacity, "New capacity must be larger than current capacity");

            const size_type oldSize = _size;

            // Allocate new block
            T* newData = static_cast<T*>(_allocator.allocate(newCap * sizeof(T)));

            // Move or copy elements
            if constexpr (std::is_nothrow_move_constructible_v<T>)
            {
                // Best path: no-throw move
                for (size_type i = 0; i < oldSize; ++i)
                {
                    new (&newData[i]) T(std::move(_data[i]));
                }
            }
            else if constexpr (std::is_copy_constructible_v<T>)
            {
                // Fallback: copy if move potentially throws
                for (size_type i = 0; i < oldSize; ++i)
                {
                    new (&newData[i]) T(_data[i]);
                }
            }
            else
            {
                static_assert(std::is_move_constructible_v<T>, "BasicList<T> requires T to be move-constructible or copy-constructible.");
            }

            // Deallocate old memory
            if (_data)
            {
                destroy_range(0, oldSize);
                _allocator.deallocate(_data, _capacity * sizeof(T));
            }

            // Install new block
            _data = newData;
            _capacity = newCap;
        }

        void destroy_range(const size_type begin, const size_type end) noexcept
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                for (size_type i = begin; i < end; ++i)
                    _data[i].~T();
            }
        }

        // Construction helpers
        void construct_at(size_type index, const T& value);
        void construct_at(size_type index, T&& value);

        template<typename... Args>
        void construct_at_emplace(size_type index, Args&&... args);
    };
}