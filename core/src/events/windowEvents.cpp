#include <dory/core/events/windowEvents.h>

namespace dory::core::events::window
{
    void EventDispatcher::fire(resources::DataContext& context, const Close& eventData)
    {
        _dispatcher.fire(context, eventData);
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, const Close&)> handler)
    {
        return _hub.attach(std::move(handler));
    }
}