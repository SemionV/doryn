#include <dory/core/events/ioEvents.h>

namespace dory::core::events::io
{
    void EventDispatcher::fire(resources::DataContext& context, const KeyCode& eventData)
    {
        _dispatcher.fire(context, eventData);
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, const KeyCode&)> handler)
    {
        return _hub.attach(std::move(handler));
    }
}