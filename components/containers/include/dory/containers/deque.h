#pragma once

namespace dory::containers
{
    template<typename T, typename TAllocator, std::size_t BlockSize = 64>
    class BasicDeque
    {
    public:
        using value_type      = T;
        using size_type       = std::size_t;
        using allocator_type  = TAllocator;
        using pointer         = T*;
        using reference       = T&;
        using const_reference = const T&;

    private:
        allocator_type& _allocator;

        pointer* _map          = nullptr;    // array of block pointers
        size_type _mapCapacity = 0;
        size_type _mapStart    = 0;          // index of first block
        size_type _mapEnd      = 0;          // index one past last block

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
            _mapStart = _mapCapacity / 2;
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
        reference at(size_type index);
        const_reference at(size_type index) const;

        reference operator[](size_type index);
        const_reference operator[](size_type index) const;

        reference front();
        reference back();

        // === Capacity ===
        [[nodiscard]] size_type size() const noexcept { return _size; }
        [[nodiscard]] bool empty() const noexcept { return _size == 0; }

        // === Modifiers ===
        void clear();

        void push_back(const T& value)
        {
            const size_type blockIndex = _size / BlockSize;

            // Create new block if needed
            reserve_map_back();
            if (_mapStart + blockIndex == _mapEnd)
            {
                _map[_mapEnd++] = allocate_block();
            }

            pointer block = _map[_mapStart + blockIndex];

            new (&block[_size % BlockSize]) T(value);

            ++_size;
        }

        void push_back(T&& value);

        void push_front(const T& value);
        void push_front(T&& value);

        void pop_back();
        void pop_front();

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
                const size_type newCap = _mapCapacity * 2;
                auto* newMap = static_cast<pointer*>(
                    _allocator.allocate(newCap * sizeof(pointer))
                );

                // Center existing blocks in new map
                const size_type offset = (newCap - _mapCapacity) / 2;

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
                const size_type newCap = _mapCapacity * 2;
                auto* newMap = static_cast<pointer*>(
                    _allocator.allocate(newCap * sizeof(pointer))
                );

                const size_type offset = (newCap - _mapCapacity) / 2;

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
    };
}