#pragma once

namespace dory::memory::dynamic
{
    //Sores status value and ref count in the most significant byte
    class BlockStatus64
    {
        constexpr static std::size_t refCountOffset = std::numeric_limits<std::size_t>::digits - std::numeric_limits<std::uint8_t>::digits;
        constexpr static std::size_t refCountMask = ~0ULL << refCountOffset;
        std::size_t _initialValue {};
        std::size_t _value {};

    public:
        explicit BlockStatus64(const std::size_t value) noexcept
        : _initialValue(value),
        _value(value)
        {}

        [[nodiscard]] std::uint8_t getRefCount() const
        {
            return _value >> refCountOffset;
        }

        void setRefCount(const std::uint8_t refCount)
        {
            std::size_t value = refCount;
            value <<= refCountOffset;
            _value &= ~refCountMask;
            _value |= value;
        }

        [[nodiscard]] std::size_t getValue() const
        {
            return _value & ~refCountMask;
        }

        void setValue(const std::size_t value)
        {
            _value = value | (_value & refCountMask);
        }

        [[nodiscard]] std::size_t getRawValue() const
        {
            return _value;
        }

        [[nodiscard]] std::size_t getInitialValue() const
        {
            return _initialValue;
        }
    };

    class BlockStatus32
    {
        std::uint32_t _value {};
        std::uint32_t _initialValue {};
        std::uint8_t _refCount {};
        std::uint8_t _initialRefCount {};

    public:
        explicit BlockStatus32(const std::uint32_t value, const uint32_t refCount) noexcept
        : _value(value),
        _initialValue(value),
        _refCount(refCount),
        _initialRefCount(refCount)
        {}

        [[nodiscard]] std::uint8_t getRefCount() const
        {
            return _refCount;
        }

        void setRefCount(const std::uint8_t refCount)
        {
            _refCount = refCount;
        }

        [[nodiscard]] std::uint32_t getValue() const
        {
            return _value;
        }

        void setValue(const std::uint32_t value)
        {
            _value = value;
        }

        [[nodiscard]] std::pair<std::uint32_t, std::uint8_t> getRawValue() const
        {
            return std::make_pair(_value, _refCount);
        }

        [[nodiscard]] std::pair<std::uint32_t, std::uint8_t> getInitialValue() const
        {
            return std::make_pair(_initialValue, _initialRefCount);
        }
    };

    template<std::size_t MachineWordSize>
    class BlockStatusStorage
    {
        std::atomic<std::uint32_t> _value {};
        std::atomic<std::uint8_t> _refCount {};

    public:
        using BlockStatusType = BlockStatus32;

        [[nodiscard]] BlockStatusType load() const
        {
            return BlockStatusType { _value.load(), _refCount.load() };
        }

        std::optional<BlockStatusType> store(const BlockStatusType& status)
        {
            std::pair<std::uint32_t, std::uint8_t> initialValue = status.getInitialValue();
            const std::pair<std::uint32_t, std::uint8_t> rawValue = status.getRawValue();

            if(!_value.compare_exchange_strong(initialValue.first, rawValue.first))
            {
                return BlockStatusType { initialValue.first, _refCount.load() };
            }

            if(!_refCount.compare_exchange_strong(initialValue.second, rawValue.second))
            {
                return BlockStatusType { _value.load(), initialValue.second };
            }

            return {};
        }
    };

    template<>
    class BlockStatusStorage<64>
    {
        std::atomic<std::size_t> _value {};

    public:
        using BlockStatusType = BlockStatus64;

        [[nodiscard]] BlockStatusType load() const
        {
            return BlockStatusType { _value.load() };
        }

        std::optional<BlockStatusType> store(const BlockStatusType& status)
        {
            std::size_t initialValue = status.getInitialValue();
            const std::size_t rawValue = status.getRawValue();
            if(!_value.compare_exchange_strong(initialValue, rawValue))
            {
                return BlockStatusType { initialValue };
            }

            return {};
        }
    };

    template<typename TBlockStatusStorage>
    struct alignas(constants::cacheLineSize) Block //alignas: avoid fake sharing and cache ping-pong between CPU cores
    {
        void* ptr {};
        std::size_t size {};
        std::size_t nextBlock {};
        TBlockStatusStorage status {};
    };

    struct BlockHandle
    {
        std::atomic<Block*> ptr {};
        std::atomic<std::uint16_t> refCounter {};
    };

    //TODO: analyze memory ordering
    class RefCounter
    {
        using CounterType = std::size_t;

        static constexpr CounterType isZeroMask = 1ULL << std::numeric_limits<CounterType>::digits - 1;
        static constexpr CounterType isHelpedMask = 1ULL << std::numeric_limits<CounterType>::digits - 2;

        std::atomic<std::size_t> _counter {};

    public:
        bool incrementIfNotZero()
        {
            return (_counter.fetch_add(1) & isZeroMask) == 0;
        }

        bool decrement()
        {
            if(_counter.fetch_sub(1) == 1)
            {
                CounterType expected = 0;
                if(_counter.compare_exchange_strong(expected, isZeroMask)
                    || (expected & isHelpedMask && _counter.exchange(isZeroMask) & isHelpedMask))
                {
                    return true;
                }
            }

            return false;
        }

        CounterType load()
        {
            auto value = _counter.load();
            if(value == 0 && _counter.compare_exchange_strong(value, isZeroMask | isHelpedMask))
            {
                return 0;
            }

            return value & isZeroMask ? 0 : value;
        }
    };

    template<typename TPolicies>
    class DynamicAllocator
    {
    public:
        using BlockStatusStorageType = typename TPolicies::BlockStatusStorageType;
        using BlockStatusType = typename BlockStatusStorageType::BlockStatusType;
        using BlockType = Block<BlockStatusStorageType>;

    private:
        constexpr static std::size_t invalidBlockHandleIndex = std::numeric_limits<std::size_t>::max();

        MemoryBlock _memory {};
        PoolAllocator<BlockType>& _blockAllocator;

        MemoryPool<BlockHandle> _blockHandlePool {};
        BitArray<std::uint64_t> _blockHandleFreeList;

        std::size_t _headBlockHandleIndex { invalidBlockHandleIndex };

    public:
        explicit DynamicAllocator(const MemoryBlock& memory,
            PoolAllocator<BlockType>& blockAllocator,
            const MemoryPool<BlockHandle>& blockHandlePool,
            const BitArray<std::uint64_t>& blockHandleFreeList) noexcept
            : _memory(memory),
            _blockAllocator(blockAllocator),
            _blockHandlePool(blockHandlePool),
            _blockHandleFreeList(blockHandleFreeList)
        {}

        template<typename T>
        ResourceHandle<T> allocate() noexcept
        {
            assert::debug(_headBlockHandleIndex != invalidBlockHandleIndex, "Head block descriptor is not allocated");

            const std::size_t align = alignof(T);
            const std::size_t size = sizeof(T);

            std::size_t blockHandleIndex = _headBlockHandleIndex;
            while(blockHandleIndex != invalidBlockHandleIndex)
            {
                std::size_t result = findFreeBlock(size, getAlignPower(align), blockHandleIndex);

                /*if(result.second)
                {
                    //TODO: this is not finished yet
                    if(result.first != invalidBlockHandleIndex)
                    {
                        auto& blockHandle = getBlockHandle(result.first);
                        Block* block = blockHandle.ptr.load();
                        assert::debug(block, "Invalid block handle");
                    }
                    else
                    {
                        blockHandleIndex = invalidBlockHandleIndex;
                    }
                }
                else
                {
                    blockHandleIndex = result.first;
                }*/
            }

            return ResourceHandle<T>{ {} };
        }

    private:
        [[nodiscard]] BlockHandle& getBlockHandle(const std::size_t blockHandle) const
        {
            assert::debug(blockHandle < _blockHandlePool.itemsCount, "Invalid block handle");
            return _blockHandlePool.pointer[blockHandle];
        }

        static int getAlignPower(std::uint16_t align) noexcept
        {
            assert::debug(align, "Alignment cannot be 0");
            assert::debug((align & align - 1) == 0, "Alignment must be a power of 2");

            int position = 0;
            while (align >>= 1)
            {
                ++position;
            }
            return position;
        }

        static std::size_t getAlignedOffset(const std::uint8_t alignPower, const DynamicBlock& block) noexcept
        {
            const auto address = reinterpret_cast<std::uintptr_t>(block.ptr);
            const std::uintptr_t alignedAddress = alignAddress(address, 1 << alignPower);
            return alignedAddress - address;
        }

        static std::size_t findFreeBlock(const std::size_t size, const std::uint8_t alignPower, const std::size_t beginHandleIndex) noexcept
        {
            BlockHandle& blockHandle = getBlockHandle(beginHandleIndex);
            BlockType* block = blockHandle.ptr.load();
            assert::debug(block, "Invalid block handle");
            BlockStatusType status = block->status.load();
        }
    };
}