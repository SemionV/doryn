#pragma once

namespace dory::memory
{
    template<typename TCounter = std::size_t>
    class ControlBlock
    {
    private:
        std::atomic<TCounter> _handlesCount = 0;
        std::atomic<TCounter> _referencesCount = 1;

    public:
        ControlBlock() = default;

        explicit ControlBlock(TCounter handlesCount):
            _handlesCount(handlesCount)
        {}

        void incrementHandlesCount()
        {
            ++_handlesCount;
        }

        TCounter decrementHandlesCount()
        {
            return --_handlesCount == 0;
        }

        void incrementReferencesCount()
        {
            ++_referencesCount;
        }

        TCounter decrementReferencesCount()
        {
            return --_referencesCount;
        }
    };

    template<typename TResource, typename TCounter = std::size_t>
    class ResourceHandle
    {
    private:
        TResource* const _resource;
        ControlBlock<TCounter>* const _controlBlock;

    public:
        explicit ResourceHandle(TResource* resource):
            _resource(resource),
            _controlBlock(new ControlBlock<TCounter>{ 1 })
        {}

        ~ResourceHandle()
        {
            if(_controlBlock->decrementHandlesCount() == 0)
            {
                if(_controlBlock->getReferencesCount() == 0)
                {
                    delete _controlBlock;
                }

                _controlBlock->decrementReferencesCount();
                delete _resource;
            }
        }
    };

    template<typename TResource, typename TCounter>
    class ResourceRef
    {
    private:
        ControlBlock<TCounter>* const _controlBlock;

    public:
        explicit ResourceRef(ControlBlock<TCounter>* controlBlock):
            _controlBlock(controlBlock)
        {
            _controlBlock->incrementReferencesCount();
        }

        ~ResourceRef()
        {
            if(_controlBlock->decrementReferencesCount() == 0)
            {
                delete _controlBlock;
            }
        }
    };
}