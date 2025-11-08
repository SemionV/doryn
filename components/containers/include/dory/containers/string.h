#pragma once

#include <cstring>
#include <dory/macros/assert.h>

namespace dory::containers
{
    template<typename TChar, typename TCharTraits, typename TAllocator>
    class BasicString
    {
    public:
        using value_type = TChar;
        using size_type = std::size_t;
        using allocator_type = TAllocator;

    private:
        TChar* _data = nullptr;
        size_type _size = 0;
        size_type _capacity = 0;
        allocator_type& _allocator;

    public:
        explicit BasicString(allocator_type& allocator) noexcept:
            _allocator(allocator)
        {}

        BasicString(const TChar* cstr, allocator_type& allocator) noexcept(false):
            _allocator(allocator)
        {
            assert::inhouse(cstr != _data, "Cannot construct string from its own data");
            assert::inhouse(cstr, "Pointer to a char string must be a valid pointer");

            _size = 0;
            while(cstr[_size] != '\0')
                ++_size;

            _capacity = _size + 1;

            _data = static_cast<TChar*>(_allocator.allocate(_capacity * sizeof(TChar)));

            TCharTraits::copy(_data, cstr, _size);
            _data[_size] = TChar('\0');
        }

        BasicString(const TChar* data, const size_type length, allocator_type& allocator):
            _allocator(allocator)
        {
            assert::inhouse(data != _data, "Cannot construct string from its own data");
            assert::inhouse(data, "Pointer to a char string must be a valid pointer");

            _size = length;
            _capacity = _size + 1;

            _data = static_cast<TChar*>(_allocator.allocate(_capacity * sizeof(TChar)));

            TCharTraits::copy(_data, data, _size);
            _data[_size] = TChar('\0');
        }

        BasicString(const BasicString& other) = delete;

        BasicString(BasicString&& other) noexcept:
        _data(other._data),
        _size(other._size),
        _capacity(other._capacity),
        _allocator(other._allocator)
        {
            other._data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }

        BasicString& operator=(BasicString&& other) noexcept
        {
            if (this != &other)
            {
                assert::inhouse(&_allocator == &other._allocator,
                    "Move assignment between strings with different allocators is not allowed");

                if (_data)
                {
                    _allocator.deallocate(_data);
                    _data = nullptr;
                }

                _data = other._data;
                _size = other._size;
                _capacity = other._capacity;

                other._data = nullptr;
                other._size = 0;
                other._capacity = 0;
            }

            return *this;
        }

        ~BasicString()
        {
            _allocator.deallocate(_data, _capacity);
        }

        // === Element access ===
        const TChar* c_str() const noexcept
        {
            return _data;
        }

        const TChar* data() const noexcept
        {
            return _data;
        }

        TChar* data() noexcept
        {
            return _data;
        }

        TChar& operator[](size_type i) noexcept
        {
            assert::inhouse(i < _capacity, "Invalid index");
            return _data[i];
        }

        const TChar& operator[](size_type i) const noexcept
        {
            assert::inhouse(i < _capacity, "Invalid index");
            return _data[i];
        }

        // === Capacity ===
        [[nodiscard]] size_type size() const noexcept
        {
            return _size;
        }

        [[nodiscard]] size_type length() const noexcept
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
            if(newCap > _capacity)
                grow(newCap);
        }

        void clear() noexcept
        {
            if (_data)
                _data[0] = TChar('\0');
            _size = 0;
        }

        void resize(const size_type newSize)
        {
            if (newSize + 1 > _capacity)
            {
                grow(newSize + 1);
            }

            if (newSize > _size)
            {
                // Zero-fill newly added characters
                TCharTraits::assign(_data + _size, newSize - _size, TChar('\0'));
            }

            _size = newSize;
            _data[_size] = TChar('\0');
        }

        // === Modifiers ===
        void append(const TChar* str, size_type len)
        {
            assert::inhouse(str, "String pointer must be valid");

            const size_type missingCapacity = len - _capacity + _size + 1;
            if(missingCapacity > 0)
                grow(_capacity + missingCapacity);

            TCharTraits::copy(_data + _size, str, len);
            _size += len;
            _data[_size] = TChar('\0');
        }

        void append(const BasicString& other)
        {
            append(other.data(), other.length());
        }

        void push_back(TChar c);
        void pop_back();
        void assign(const TChar* str, size_type len);
        void shrink_to_fit();

        // === Comparison ===
        bool operator==(const BasicString& rhs) const noexcept;
        bool operator!=(const BasicString& rhs) const noexcept;
        bool operator<(const BasicString& rhs) const noexcept;

        // === Conversions ===
        std::basic_string_view<TChar> view() const noexcept;
        const TChar* c_str() noexcept;
        operator std::basic_string_view<TChar>() const noexcept;

        // === Allocator access ===
        allocator_type& getAllocator() noexcept
        {
            return _allocator;
        }

    private:
        void grow(const size_type newCap)
        {
            assert::inhouse(newCap > _capacity, "New capacity must be larger than current capacity");

            auto* newData = static_cast<TChar*>(_allocator.allocate(newCap * sizeof(TChar)));

            if (_data)
            {
                TCharTraits::copy(newData, _data, _size);
                newData[_size] = TChar('\0');
                _allocator.deallocate(_data);
            }

            _data = newData;
            _capacity = newCap;
        }
    };
}
