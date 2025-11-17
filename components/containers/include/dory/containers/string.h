#pragma once

#include <cstring>
#include <dory/macros/assert.h>
#include "crc32.h"

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
        struct HeapStringData
        {
            size_type size = 0;
            TChar* data = nullptr;
            size_type capacity = 0;
        };

        template<std::size_t SSO_THRESHOLD>
        struct LocalStringData
        {
            TChar size = 0;
            TChar data[SSO_THRESHOLD];
        };


        static constexpr size_type STORAGE_SIZE = sizeof(HeapStringData);
        static constexpr size_type SSO_THRESHOLD = (STORAGE_SIZE / sizeof(TChar)) - 1;

        allocator_type& _allocator;

        union
        {
            HeapStringData _heapData;
            LocalStringData<SSO_THRESHOLD> _localData;
        };

        [[nodiscard]] bool isHeapStorage() const noexcept
        {
            return (_localData.size & 1) == 1;
        }

        [[nodiscard]] bool isLocalStorage() const noexcept
        {
            return !isHeapStorage();
        }

        void setHeapMode() noexcept
        {
            _localData.size = _localData.size | 1;
        }

        void setLocalMode() noexcept
        {
            _localData.size = _localData.size & ~1;
        }

        [[nodiscard]] size_type getHeapSize() const
        {
            return static_cast<size_type>(_heapData.size >> 1);
        }

        [[nodiscard]] size_type getLocalSize() const
        {
            return SSO_THRESHOLD - (static_cast<size_type>(_localData.size) >> 1);
        }

        void setLocalSize(const size_type len) noexcept
        {
            assert::debug(len <= SSO_THRESHOLD, "Invalid storage mode");
            _localData.size = static_cast<TChar>((SSO_THRESHOLD - len) << 1); // LSB = 0
        }

        void setHeapSize(const size_type len) noexcept
        {
            _heapData.size = (len << 1) | 1; // LSB = 1
        }

        void setSize(const size_type size)
        {
            if(isHeapStorage())
            {
                setHeapSize(size);
            }
            else
            {
                setLocalSize(size);
            }
        }

        TChar* getData()
        {
            return isHeapStorage() ? _heapData.data : _localData.data;
        }

        const TChar* getData() const noexcept
        {
            return isHeapStorage() ? _heapData.data : _localData.data;
        }

        [[nodiscard]] size_type getCapacity() const
        {
            return isHeapStorage() ? _heapData.capacity : SSO_THRESHOLD;
        }

        [[nodiscard]] size_type getSize() const
        {
            return isHeapStorage() ? getHeapSize() : getLocalSize();
        }

        void initFromCStr(const TChar* data, const size_type length)
        {
            const size_type capacity = length + 1;

            if (length >= SSO_THRESHOLD)
            {
                setHeapSize(length);
                setHeapMode();

                _heapData.capacity = capacity;
                _heapData.data = static_cast<TChar*>(_allocator.allocate(capacity * sizeof(TChar)));
                TCharTraits::copy(_heapData.data, data, length);
                _heapData.data[length] = TChar('\0');
            }
            else
            {
                setLocalMode();
                setLocalSize(length);
                TCharTraits::copy(_localData.data, data, length);
                _localData.data[length] = TChar('\0');
            }
        }

        void initFromOther(BasicString&& other)
        {
            if(other.isHeapStorage())
            {
                _heapData.data = other._heapData.data;
                _heapData.capacity = other._heapData.capacity;
                _heapData.size = other._heapData.size;

                other._heapData.data = nullptr;
                other._heapData.size = 0;
                other._heapData.capacity = 0;
            }
            else
            {
                TCharTraits::copy(_localData.data, other._localData.data, SSO_THRESHOLD);
                _localData.data[other.getLocalSize()] = TChar('\0');
                _localData.size = other._localData.size;
            }
        }

    public:
        explicit BasicString(allocator_type& allocator) noexcept:
            _allocator(allocator)
        {
            setLocalMode();
            setLocalSize(0);
        }

        BasicString(const TChar* cstr, allocator_type& allocator) noexcept(false):
            _allocator(allocator)
        {
            assert::inhouse(cstr != _heapData.data, "Cannot construct string from its own data");
            assert::inhouse(cstr, "Pointer to a char string must be a valid pointer");

            if (!cstr)
            {
                setLocalMode();
                setLocalSize(0);
                _localData.data[0] = TChar('\0');
                return;
            }

            size_type size = 0;
            while(cstr[size] != '\0')
                ++size;

            initFromCStr(cstr, size);
        }

        BasicString(const TChar* data, const size_type length, allocator_type& allocator):
            _allocator(allocator)
        {
            assert::inhouse(data != _heapData.data, "Cannot construct string from its own data");
            assert::inhouse(data, "Pointer to a char string must be a valid pointer");

            if (!data)
            {
                setLocalMode();
                setLocalSize(0);
                _localData.data[0] = TChar('\0');
                return;
            }

            initFromCStr(data, length);
        }

        BasicString(const BasicString& other) = delete;

        BasicString(BasicString&& other) noexcept:
        _allocator(other._allocator)
        {
            initFromOther(std::forward<BasicString>(other));
        }

        BasicString& operator=(BasicString&& other) noexcept
        {
            if (this != &other)
            {
                assert::inhouse(&_allocator == &other._allocator,
                    "Move assignment between strings with different allocators is not allowed");

                if (isHeapStorage() && _heapData.data)
                {
                    _allocator.deallocate(_heapData.data, _heapData.capacity * sizeof(TChar));
                    _heapData.data = nullptr;
                }

                initFromOther(std::forward<BasicString>(other));
            }

            return *this;
        }

        ~BasicString()
        {
            if(isHeapStorage())
            {
                _allocator.deallocate(_heapData.data, _heapData.capacity * sizeof(TChar));
            }
        }

        // === Element access ===
        const TChar* c_str() const noexcept
        {
            return getData();
        }

        const TChar* data() const noexcept
        {
            return getData();
        }

        TChar* data() noexcept
        {
            return getData();
        }

        TChar& operator[](size_type i) noexcept
        {
            assert::inhouse(i < getCapacity(), "Invalid index");
            return getData()[i];
        }

        const TChar& operator[](size_type i) const noexcept
        {
            assert::inhouse(i < getCapacity(), "Invalid index");
            return getData()[i];
        }

        // === Capacity ===
        [[nodiscard]] size_type size() const noexcept
        {
            return getSize();
        }

        [[nodiscard]] size_type length() const noexcept
        {
            return getSize();
        }

        [[nodiscard]] size_type capacity() const noexcept
        {
            return getCapacity();
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return getSize() == 0;
        }

        void reserve(const size_type newCap)
        {
            if(newCap > getCapacity())
                grow(newCap);
        }

        void clear() noexcept
        {
            if(isHeapStorage())
            {
                if (_heapData.data)
                    _heapData.data[0] = TChar('\0');
                setHeapSize(0);
            }
            else
            {
                _localData.data[0] = TChar('\0');
                setLocalSize(0);
            }
        }

        void resize(const size_type newSize)
        {
            if (newSize + 1 > getCapacity())
            {
                grow(newSize + 1);
            }

            const size_type size = getSize();
            if (newSize > size)
            {
                // Zero-fill newly added characters
                TChar* data = getData();
                TCharTraits::assign(data + size, newSize - size, TChar('\0'));
            }

            if(isHeapStorage())
            {
                setHeapSize(newSize);
                _heapData.data[newSize] = TChar('\0');
            }
            else
            {
                setLocalSize(newSize);
                _localData.data[newSize] = TChar('\0');
            }
        }

        // === Modifiers ===
        void append(const TChar* str, size_type len)
        {
            assert::inhouse(str, "String pointer must be valid");

            const size_type capacity = getCapacity();
            size_type size = getSize();

            const size_type missingCapacity = len - capacity + size + 1;
            if(missingCapacity > 0)
                grow(capacity + missingCapacity);

            TChar* data = getData();

            TCharTraits::copy(data + size, str, len);

            size += len;
            setSize(size);
            data[size] = TChar('\0');
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
            const size_type capacity = getCapacity();
            size_type size = getSize();

            const size_type missingCapacity = 2 - capacity + size;
            if(missingCapacity > 0)
                grow(capacity + missingCapacity);

            TChar* data = getData();

            data[size] = c;
            ++size;
            setSize(size);
            data[size] = TChar('\0');
        }

        void pop_back()
        {
            size_type size = getSize();

            if(size > 0)
            {
                --size;
                TChar* data = getData();
                data[size] = TChar('\0');
                setSize(size);
            }
        }

        void assign(const TChar* str, size_type len)
        {
            assert::inhouse(str, "String pointer must be valid");

            const size_type capacity = getCapacity();

            if(len + 1 > capacity)
                grow(len + 1);

            TChar* data = getData();

            TCharTraits::copy(data, str, len);
            setSize(len);
            data[len] = TChar('\0');
        }

        void shrink_to_fit()
        {
            if(isHeapStorage())
            {
                const size_type size = getHeapSize();
                if(size + 1 < _heapData.capacity) // +1 for null terminator
                {
                    auto* newData = static_cast<TChar*>(_allocator.allocate((size + 1) * sizeof(TChar)));
                    TCharTraits::copy(newData, _heapData.data, size);
                    newData[size] = TChar('\0');

                    _allocator.deallocate(_heapData.data, _heapData.capacity * sizeof(TChar));
                    _heapData.data = newData;
                    _heapData.capacity = size + 1;
                }
            }
        }

        // === Comparison ===
        bool operator==(const BasicString& rhs) const noexcept
        {
            const size_type size = getSize();
            const size_type rhsSize = rhs.getSize();

            if(size != rhsSize)
                return false;

            const TChar* data = getData();
            const TChar* rhsData = rhs.getData();

            for(size_type i = 0; i < size; ++i)
            {
                if(data[i] != rhsData[i])
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
            const size_type size = getSize();
            const size_type rhsSize = rhs.getSize();
            const TChar* data = getData();
            const TChar* rhsData = rhs.getData();

            const size_type minSize = size < rhsSize ? size : rhsSize;

            for(size_type i = 0; i < minSize; ++i)
            {
                if(data[i] < rhsData[i]) return true;
                if(data[i] > rhsData[i]) return false;
            }
            return size < rhsSize;
        }

        // === Conversions ===
        std::basic_string_view<TChar> view() const noexcept
        {
            const size_type size = getSize();
            const TChar* data = getData();
            return std::basic_string_view<TChar>(data, size);
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

        [[nodiscard]] uint32_t crc32(const CRC32Table& table) const noexcept
        {
            return dory::containers::CRC32::compute(getData(), getSize(), table);
        }

    private:
        void grow(const size_type newCap)
        {
            if (isHeapStorage())
                assert::inhouse(newCap > _heapData.capacity, "New capacity must be larger than current capacity");
            else
                assert::inhouse(newCap > SSO_THRESHOLD, "New capacity must exceed SSO threshold");

            if(newCap > SSO_THRESHOLD)
            {
                auto* newData = static_cast<TChar*>(_allocator.allocate(newCap * sizeof(TChar)));

                size_type size = 0;
                if (isHeapStorage())
                {
                    size = getHeapSize();
                    if(_heapData.data)
                    {
                        TCharTraits::copy(newData, _heapData.data, size);
                        _allocator.deallocate(_heapData.data, _heapData.capacity * sizeof(TChar));
                    }
                }
                else
                {
                    size = getLocalSize();
                    TCharTraits::copy(newData, _localData.data, size);
                }

                if (!newData) return; // allocation failure should throw or assert
                newData[size] = TChar('\0');
                _heapData.data = newData;
                _heapData.capacity = newCap;
                setHeapMode();
                setHeapSize(size);
            }
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
