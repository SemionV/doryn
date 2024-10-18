#include <dory/core/events/applicationEvents.h>

namespace dory::core::events::application
{
    void EventDispatcher::fire(resources::DataContext& context, const Exit& eventData)
    {
        _dispatcher.fire(context, eventData);
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, const Exit&)> handler)
    {
        return _hub.attach(std::move(handler));
    }
}