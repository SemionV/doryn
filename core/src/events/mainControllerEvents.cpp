#include <dory/core/events/mainControllerEvents.h>

namespace dory::core::events::mainController
{
    void EventDispatcher::fire(resources::DataContext& context, const Initialize& eventData)
    {
        _dispatcher.fire(context, eventData);
    }

    void EventDispatcher::fire(resources::DataContext& context, const Stop& eventData)
    {
        _dispatcher.fire(context, eventData);
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, const Initialize&)> handler)
    {
        return _hub.attach(std::move(handler));
    }

    std::size_t EventDispatcher::attach(std::function<void(resources::DataContext&, const Stop&)> handler)
    {
        return _hub.attach(std::move(handler));
    }
}