#pragma once

#include <atomic>
#include <dory/data-structures/function.h>

namespace dory::core::event_system
{
    template<typename TKey = std::size_t>
    class EventHandlerKeyGenerator
    {
        static_assert(std::atomic<TKey>::is_always_lock_free, "HandlerKeyType atomic is not lock-free on this platform. Pick another numeric type.");

    public:
        using HandlerKeyType = TKey;

    private:
        std::atomic<HandlerKeyType> _currentKey { 1 }; //0 can be used as sentinel

    public:
        HandlerKeyType generate()
        {
            return _currentKey.fetch_add(1, std::memory_order::relaxed);
        }
    };

    struct EventHandlerKey
    {

    };

    template<typename TKeyGenerator, class... TParams>
    class Event
    {
    public:

    };
}