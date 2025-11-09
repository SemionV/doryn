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
        static constexpr size_type SSO_THRESHOLD = 23;

        allocator_type& _allocator;

        union
        {
            struct
            {
                TChar* data;
                size_type capacity = 0;
                size_type size = 0;
            } _heapData;
            TChar _ssoData[SSO_THRESHOLD + 1];
        };

        /*TChar* _data = nullptr;
        size_type _size = 0;
        size_type _capacity = 0;*/

    public:
        explicit BasicString(allocator_type& allocator) noexcept:
            _allocator(allocator)
        {}

        BasicString(const TChar* cstr, allocator_type& allocator) noexcept(false):
            _allocator(allocator)
        {
            assert::inhouse(cstr != _heapData.data, "Cannot construct string from its own data");
            assert::inhouse(cstr, "Pointer to a char string must be a valid pointer");

            _heapData.size = 0;
            while(cstr[_heapData.size] != '\0')
                ++_heapData.size;

            _heapData.capacity = _heapData.size + 1;

            _heapData.data = static_cast<TChar*>(_allocator.allocate(_heapData.capacity * sizeof(TChar)));

            TCharTraits::copy(_heapData.data, cstr, _heapData.size);
            _heapData.data[_heapData.size] = TChar('\0');
        }

        BasicString(const TChar* data, const size_type length, allocator_type& allocator):
            _allocator(allocator)
        {
            assert::inhouse(data != _heapData.data, "Cannot construct string from its own data");
            assert::inhouse(data, "Pointer to a char string must be a valid pointer");

            _heapData.size = length;
            _heapData.capacity = _heapData.size + 1;

            _heapData.data = static_cast<TChar*>(_allocator.allocate(_heapData.capacity * sizeof(TChar)));

            TCharTraits::copy(_heapData.data, data, _heapData.size);
            _heapData.data[_heapData.size] = TChar('\0');
        }

        BasicString(const BasicString& other) = delete;

        BasicString(BasicString&& other) noexcept:
        _heapData(other._heapData),
        _allocator(other._allocator)
        {
            other._heapData.data = nullptr;
            other._heapData.size = 0;
            other._heapData.capacity = 0;
        }

        BasicString& operator=(BasicString&& other) noexcept
        {
            if (this != &other)
            {
                assert::inhouse(&_allocator == &other._allocator,
                    "Move assignment between strings with different allocators is not allowed");

                if (_heapData.data)
                {
                    _allocator.deallocate(_heapData.data);
                    _heapData.data = nullptr;
                }

                _heapData.data = other._heapData.data;
                _heapData.size = other._heapData.size;
                _heapData.capacity = other._heapData.capacity;

                other._heapData.data = nullptr;
                other._heapData.size = 0;
                other._heapData.capacity = 0;
            }

            return *this;
        }

        ~BasicString()
        {
            _allocator.deallocate(_heapData.data, _heapData.capacity);
        }

        // === Element access ===
        const TChar* c_str() const noexcept
        {
            return _heapData.data;
        }

        const TChar* data() const noexcept
        {
            return _heapData.data;
        }

        TChar* data() noexcept
        {
            return _heapData.data;
        }

        TChar& operator[](size_type i) noexcept
        {
            assert::inhouse(i < _heapData.capacity, "Invalid index");
            return _heapData.data[i];
        }

        const TChar& operator[](size_type i) const noexcept
        {
            assert::inhouse(i < _heapData.capacity, "Invalid index");
            return _heapData.data[i];
        }

        // === Capacity ===
        [[nodiscard]] size_type size() const noexcept
        {
            return _heapData.size;
        }

        [[nodiscard]] size_type length() const noexcept
        {
            return _heapData.size;
        }

        [[nodiscard]] size_type capacity() const noexcept
        {
            return _heapData.capacity;
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return _heapData.size == 0;
        }

        void reserve(const size_type newCap)
        {
            if(newCap > _heapData.capacity)
                grow(newCap);
        }

        void clear() noexcept
        {
            if (_heapData.data)
                _heapData.data[0] = TChar('\0');
            _heapData.size = 0;
        }

        void resize(const size_type newSize)
        {
            if (newSize + 1 > _heapData.capacity)
            {
                grow(newSize + 1);
            }

            if (newSize > _heapData.size)
            {
                // Zero-fill newly added characters
                TCharTraits::assign(_heapData.data + _heapData.size, newSize - _heapData.size, TChar('\0'));
            }

            _heapData.size = newSize;
            _heapData.data[_heapData.size] = TChar('\0');
        }

        // === Modifiers ===
        void append(const TChar* str, size_type len)
        {
            assert::inhouse(str, "String pointer must be valid");

            const size_type missingCapacity = len - _heapData.capacity + _heapData.size + 1;
            if(missingCapacity > 0)
                grow(_heapData.capacity + missingCapacity);

            TCharTraits::copy(_heapData.data + _heapData.size, str, len);
            _heapData.size += len;
            _heapData.data[_heapData.size] = TChar('\0');
        }

        void append(const TChar* str)
        {
            append(str, TCharTraits::length(str));
        }

        void append(const BasicString& other)
        {
            append(other.data(), other.length());
        }

        void push_back(TChar c)
        {
            const size_type missingCapacity = 2 - _heapData.capacity + _heapData.size;
            if(missingCapacity > 0)
                grow(_heapData.capacity + missingCapacity);

            _heapData.data[_heapData.size] = c;
            ++_heapData.size;
            _heapData.data[_heapData.size] = TChar('\0');
        }

        void pop_back()
        {
            if(_heapData.size > 0)
            {
                --_heapData.size;
                _heapData.data[_heapData.size] = TChar('\0');
            }
        }

        void assign(const TChar* str, size_type len)
        {
            assert::inhouse(str, "String pointer must be valid");

            if(len + 1 > _heapData.capacity)
                grow(len + 1);

            TCharTraits::copy(_heapData.data, str, len);
            _heapData.size = len;
            _heapData.data[_heapData.size] = TChar('\0');
        }

        void shrink_to_fit()
        {
            if(_heapData.size + 1 < _heapData.capacity) // +1 for null terminator
            {
                auto* newData = static_cast<TChar*>(_allocator.allocate((_heapData.size + 1) * sizeof(TChar)));
                TCharTraits::copy(newData, _heapData.data, _heapData.size);
                newData[_heapData.size] = TChar('\0');

                _allocator.deallocate(_heapData.data, _heapData.capacity);
                _heapData.data = newData;
                _heapData.capacity = _heapData.size + 1;
            }
        }

        // === Comparison ===
        bool operator==(const BasicString& rhs) const noexcept
        {
            if(_heapData.size != rhs._heapData.size)
                return false;

            for(size_type i = 0; i < _heapData.size; ++i)
            {
                if(_heapData.data[i] != rhs._heapData.data[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const BasicString& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator<(const BasicString& rhs) const noexcept
        {
            const size_type minSize = _heapData.size < rhs._heapData.size ? _heapData.size : rhs._heapData.size;

            for(size_type i = 0; i < minSize; ++i)
            {
                if(_heapData.data[i] < rhs._heapData.data[i]) return true;
                if(_heapData.data[i] > rhs._heapData.data[i]) return false;
            }
            return _heapData.size < rhs._heapData.size;
        }

        // === Conversions ===
        std::basic_string_view<TChar> view() const noexcept
        {
            return std::basic_string_view<TChar>(_heapData.data, _heapData.size);
        }

        const TChar* c_str() noexcept
        {
            return _heapData.data ? _heapData.data : "";
        }

        explicit operator std::basic_string_view<TChar>() const noexcept
        {
            return view();
        }

        // === Allocator access ===
        allocator_type& getAllocator() noexcept
        {
            return _allocator;
        }

    private:
        void grow(const size_type newCap)
        {
            assert::inhouse(newCap > _heapData.capacity, "New capacity must be larger than current capacity");

            auto* newData = static_cast<TChar*>(_allocator.allocate(newCap * sizeof(TChar)));

            if (_heapData.data)
            {
                TCharTraits::copy(newData, _heapData.data, _heapData.size);
                newData[_heapData.size] = TChar('\0');
                _allocator.deallocate(_heapData.data);
            }

            _heapData.data = newData;
            _heapData.capacity = newCap;
        }

        bool isSSO() noexcept
        {
            return _heapData.size >> 63 == 1;
        }

        void setSSO(const bool isSSO) noexcept
        {
            /*size_type mask = isSSO;
            mask = mask << 63;

            _heapData*/
        }
    };

    template<typename TChar, typename TCharTraits, typename TAllocator>
    BasicString<TChar, TCharTraits, TAllocator> operator+(
        const BasicString<TChar, TCharTraits, TAllocator>& lhs,
        const BasicString<TChar, TCharTraits, TAllocator>& rhs)
    {
        BasicString result(lhs.getAllocator());
        result.reserve(lhs.size() + rhs.size() + 1);
        result.append(lhs);
        result.append(rhs);
        return result;
    }
}
