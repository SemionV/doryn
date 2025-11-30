#pragma once

#include <iterator>

namespace dory::containers
{
    template<typename T>
    class DequeIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;
        using difference_type   = std::ptrdiff_t;

        static constexpr std::size_t blockSize = 64; // example — match your deque

    private:
        T* cur;        // current element
        T* first;      // begin of block
        T* last;       // end of block
        T** node;      // pointer to block in map

    public:
        DequeIterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

        DequeIterator(T** nodePtr, T* firstPtr, T* lastPtr, T* curPtr)
            : cur(curPtr), first(firstPtr), last(lastPtr), node(nodePtr) {}

        reference operator*() const noexcept { return *cur; }
        pointer operator->() const noexcept { return cur; }

        // -----------------------------
        // Increment / Decrement
        // -----------------------------

        DequeIterator& operator++() noexcept
        {
            ++cur;
            if (cur == last)
                setNode(node + 1);
            return *this;
        }

        DequeIterator operator++(int) noexcept
        {
            DequeIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        DequeIterator& operator--() noexcept
        {
            if (cur == first)
                setNode(node - 1);
            --cur;
            return *this;
        }

        DequeIterator operator--(int) noexcept
        {
            DequeIterator tmp = *this;
            --(*this);
            return tmp;
        }

        // -----------------------------
        // Random access movement
        // -----------------------------

        DequeIterator& operator+=(difference_type n)
        {
            difference_type offset = n + (cur - first);

            if (offset >= 0 && offset < static_cast<difference_type>(blockSize))
            {
                cur += n;
                return *this;
            }

            difference_type nodeOffset =
                offset > 0 ?
                    offset / static_cast<difference_type>(blockSize)
                  : -static_cast<difference_type>((-offset - 1) / blockSize) - 1;

            setNode(node + nodeOffset);
            cur = first + (offset - nodeOffset * static_cast<difference_type>(blockSize));
            return *this;
        }

        DequeIterator operator+(difference_type n) const
        {
            DequeIterator tmp = *this;
            return tmp += n;
        }

        DequeIterator& operator-=(difference_type n)
        {
            return *this += -n;
        }

        DequeIterator operator-(difference_type n) const
        {
            DequeIterator tmp = *this;
            return tmp -= n;
        }

        difference_type operator-(const DequeIterator& rhs) const
        {
            const difference_type blockDiff = (node - rhs.node);
            const difference_type curDiff   = (cur - first);
            const difference_type rhsDiff   = (rhs.cur - rhs.first);

            return blockDiff * static_cast<difference_type>(blockSize)
                   + curDiff - rhsDiff;
        }

        // -----------------------------
        // Comparisons
        // -----------------------------

        bool operator==(const DequeIterator& rhs) const noexcept
        {
            return cur == rhs.cur;
        }

        bool operator!=(const DequeIterator& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator<(const DequeIterator& rhs) const noexcept
        {
            return (*this - rhs) < 0;
        }

        bool operator>(const DequeIterator& rhs) const noexcept
        {
            return rhs < *this;
        }

        bool operator<=(const DequeIterator& rhs) const noexcept
        {
            return !(*this > rhs);
        }

        bool operator>=(const DequeIterator& rhs) const noexcept
        {
            return !(*this < rhs);
        }

    private:
        void setNode(T** newNode)
        {
            node = newNode;
            first = *node;
            last = first + blockSize;
        }
    };

    template<typename T>
    class ConstDequeIterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using pointer           = const T*;
        using reference         = const T&;
        using difference_type   = std::ptrdiff_t;

    private:
        DequeIterator<T> _it;

    public:
        ConstDequeIterator() = default;
        explicit ConstDequeIterator(const DequeIterator<T>& it) : _it(it) {}

        reference operator*()  const noexcept { return *_it; }
        pointer   operator->() const noexcept { return _it.operator->(); }

        // All operations forward to underlying iterator
        ConstDequeIterator& operator++() noexcept { ++_it; return *this; }
        ConstDequeIterator operator++(int) noexcept { auto tmp=*this; ++_it; return tmp; }
        ConstDequeIterator& operator--() noexcept { --_it; return *this; }
        ConstDequeIterator operator--(int) noexcept { auto tmp=*this; --_it; return tmp; }

        ConstDequeIterator& operator+=(difference_type n) { _it += n; return *this; }
        ConstDequeIterator operator+(difference_type n) const { auto tmp=*this; return tmp+=n; }
        ConstDequeIterator& operator-=(difference_type n) { _it -= n; return *this; }
        ConstDequeIterator operator-(difference_type n) const { auto tmp=*this; return tmp-=n; }

        difference_type operator-(const ConstDequeIterator& rhs) const { return _it - rhs._it; }

        bool operator==(const ConstDequeIterator& rhs) const noexcept { return _it == rhs._it; }
        bool operator!=(const ConstDequeIterator& rhs) const noexcept { return !(*this == rhs); }
        bool operator<(const ConstDequeIterator& rhs) const noexcept { return _it < rhs._it; }
        bool operator>(const ConstDequeIterator& rhs) const noexcept { return rhs < *this; }
        bool operator<=(const ConstDequeIterator& rhs) const noexcept { return !(*this > rhs); }
        bool operator>=(const ConstDequeIterator& rhs) const noexcept { return !(*this < rhs); }
    };

    template<typename T, typename TAllocator, std::size_t BlockSize = 64>
    class BasicDeque
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using allocator_type  = TAllocator;
        using pointer         = T*;
        using const_pointer         = T*;
        using reference       = T&;
        using const_reference = const T&;

    private:
        allocator_type& _allocator;

        pointer* _map          = nullptr;    // array of block pointers
        size_type _mapCapacity = 0;
        size_type _mapStart    = 0;          // index of first block
        size_type _mapEnd      = 0;          // index one past last block
        size_type _startOffset = 0;          // index of first element in _map[_mapStart]
        size_type _endOffset = 0;            // index of last element in _map[_mapEnd]

        size_type _size        = 0;

    public:
        // === Constructor / Destructor ===
        explicit BasicDeque(allocator_type& alloc):
            _allocator(alloc)
        {
            // Start with minimal map (3 blocks for safety)
            _mapCapacity = 8;
            _map = static_cast<pointer*>(
                _allocator.allocate(_mapCapacity * sizeof(pointer))
            );

            // All blocks initially null
            for(size_type i = 0; i < _mapCapacity; ++i)
                _map[i] = nullptr;

            // Start in the middle to allow growth both ways
            _mapStart = _mapCapacity >> 1;
            _mapEnd   = _mapStart;
        }

        ~BasicDeque()
        {
            // Destroy all elements
            if (_size > 0)
            {
                for(size_type i = 0; i < _size; ++i)
                {
                    pointer block = block_for_index(i);
                    (block[block_offset(i)]).~T();
                }
            }

            // Deallocate blocks
            for(size_type i = _mapStart; i < _mapEnd; ++i)
            {
                if (_map[i])
                    deallocate_block(_map[i]);
            }

            // Deallocate map
            _allocator.deallocate(_map, _mapCapacity * sizeof(pointer));
        }

        // === Element Access ===
        reference at(const size_type index)
        {
            return operator[](index);
        }

        const_reference at(const size_type index) const
        {
            return operator[](index);
        }

        reference operator[](const size_type index)
        {
            return access(*this, index);
        }

        const_reference operator[](const size_type index) const
        {
            return access(*this, index);
        }

        reference front()
        {
            return access(*this, 0);
        }
        reference back()
        {
            return access(*this, _size - 1);
        }

        // === Capacity ===
        [[nodiscard]] size_type size() const noexcept { return _size; }
        [[nodiscard]] bool empty() const noexcept { return _size == 0; }

        // === Modifiers ===
        void clear();

        void push_back(T&& value)
        {
            // If current block is full, move into a new block
            if (_endOffset == BlockSize)
            {
                reserve_map_back();

                // Move to next block
                _mapEnd++;

                // Allocate block if necessary
                if (_map[_mapEnd] == nullptr)
                    _map[_mapEnd] = allocate_block();

                _endOffset = 0;
            }

            if (_map[_mapEnd] == nullptr)
                _map[_mapEnd] = allocate_block();

            storeValue(&_map[_mapEnd][_endOffset], std::forward<T>(value));

            _endOffset++;
            _size++;
        }

        void push_front(const T& value);
        void push_front(T&& value);

        void pop_back();
        void pop_front();

        using iterator = DequeIterator<T>;
        using const_iterator = ConstDequeIterator<T>;

        iterator begin()
        {
            return iterator(
                _map + _mapStart,
                _map[_mapStart],
                _map[_mapStart] + BlockSize,
                _map[_mapStart] + _startOffset
            );
        }

        iterator end()
        {
            return iterator(
                _map + _mapEnd,
                _map[_mapEnd],
                _map[_mapEnd] + BlockSize,
                _map[_mapEnd] + _endOffset
            );
        }

        const_iterator begin() const { return const_iterator(const_cast<BasicDeque*>(this)->begin()); }
        const_iterator end() const   { return const_iterator(const_cast<BasicDeque*>(this)->end()); }

        const_iterator cbegin() const { return begin(); }
        const_iterator cend() const   { return end(); }

    private:
        // === Internal helpers ===

        pointer allocate_block()
        {
            return static_cast<pointer>(
                _allocator.allocate(BlockSize * sizeof(T))
            );
        }

        void deallocate_block(pointer block)
        {
            _allocator.deallocate(block, BlockSize * sizeof(T));
        }

        // Ensure capacity in map for front/back expansion
        void reserve_map_front()
        {
            if (_mapStart == 0)
            {
                // Need to reallocate map
                const size_type newCap = _mapCapacity << 1;
                auto* newMap = static_cast<pointer*>(
                    _allocator.allocate(newCap * sizeof(pointer))
                );

                // Center existing blocks in new map
                const size_type offset = (newCap - _mapCapacity) >> 1;

                for (size_type i = 0; i < newCap; ++i)
                    newMap[i] = nullptr;

                for (size_type i = 0; i < _mapCapacity; ++i)
                    newMap[i + offset] = _map[i];

                _allocator.deallocate(_map, _mapCapacity * sizeof(pointer));

                _map = newMap;
                _mapStart += offset;
                _mapEnd   += offset;
                _mapCapacity = newCap;
            }
        }

        void reserve_map_back()
        {
            if (_mapEnd == _mapCapacity)
            {
                const size_type newCap = _mapCapacity << 1;
                auto* newMap = static_cast<pointer*>(
                    _allocator.allocate(newCap * sizeof(pointer))
                );

                const size_type offset = (newCap - _mapCapacity) >> 1;

                for(size_type i = 0; i < newCap; ++i)
                    newMap[i] = nullptr;

                for(size_type i = 0; i < _mapCapacity; ++i)
                    newMap[i + offset] = _map[i];

                _allocator.deallocate(_map, _mapCapacity * sizeof(pointer));

                _map = newMap;
                _mapStart += offset;
                _mapEnd   += offset;
                _mapCapacity = newCap;
            }
        }

        // Helper: convert logical index → block + offset
        pointer block_for_index(const size_type index) const
        {
            const size_type blockIndex = index / BlockSize;
            return _map[_mapStart + blockIndex];
        }

        [[nodiscard]] static size_type block_offset(const size_type index)
        {
            return index % BlockSize;
        }

        void storeValue(T* ptr, const T& value)
        {
            new (&_map[_mapEnd][_endOffset]) T(value);
        }

        void storeValue(T* ptr, T&& value)
        {
            // Construct element in-place using move-constructor
            new (ptr) T(std::move(value));
        }

        template <typename Self>
        static auto access(Self& self, size_type index)
           -> std::conditional_t<std::is_const_v<Self>, const_reference, reference>
        {
            assert::inhouse(index < self._size, "access: index out of range");

            const size_type global = self._startOffset + index;

            const size_type blockIndex   = global / BlockSize;
            const size_type elementIndex = global % BlockSize;

            using Pointer = std::conditional_t<
                std::is_const_v<Self>,
                const_pointer,
                pointer
            >;

            Pointer block = self._map[self._mapStart + blockIndex];

            assert::debug(block != nullptr, "Internal error: null block pointer");

            return block[elementIndex];
        }
    };
}