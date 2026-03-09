#pragma once

#include <dory/interface.h>

namespace dory::data_structures::memory_reclamation
{
    class Janitor: Interface
    {
    public:
        virtual void cleanup(void* ptr) = 0;
    };

    template<typename T, typename TAllocator>
    class ObjectJanitor: public Janitor
    {
    private:
        TAllocator& _allocator;

    public:
        explicit ObjectJanitor(TAllocator& allocator):
            _allocator(allocator)
        {}

        void cleanup(void* ptr) override
        {
            _allocator.deallocateObject(static_cast<T*>(ptr));
        }
    };
}