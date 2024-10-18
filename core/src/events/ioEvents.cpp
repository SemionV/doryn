#include <dory/core/events/ioEvents.h>

namespace dory::core::events::io
{
    void EventDispatcher::fireAll(resources::DataContext& context)
    {
        _dispatcher.fireAll(context);
    }

    void EventDispatcher::charge(KeyPressEvent eventData)
    {
        _dispatcher.charge(eventData);
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, KeyPressEvent&)> handler)
    {
        return _hub.attach(std::move(handler));
    }
}