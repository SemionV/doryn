#include <dory/core/events/scriptEvents.h>

namespace dory::core::events::script
{
    void EventDispatcher::fire(resources::DataContext& context, const Run& eventData)
    {
        _dispatcher.fire(context, eventData);
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, const Run&)> handler)
    {
        return _hub.attach(std::move(handler));
    }
}