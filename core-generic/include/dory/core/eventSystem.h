#pragma once

#include <dory/data-structures/function.h>
#include <dory/data-structures/containers/lockfree/freeListArray.h>

namespace dory::core::event_system
{
    using IndexType = std::uint32_t;

    struct EventHandlerKey
    {
        IndexType index;
        IndexType generation;
    };

    template<typename... TParams>
    class Event
    {
    private:
        //using ListType = data_structures::containers::lockfree::freelist::FreeListArray<Payload, AllocatorType, 32, 1048576>;

    public:
        using HandlerType = data_structures::function::Function<void(TParams...)>;

        template<typename F>
        EventHandlerKey attachHandler(F&& handler)
        {

        }
    };
}