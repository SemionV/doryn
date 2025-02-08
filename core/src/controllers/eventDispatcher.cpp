#include <dory/core/registry.h>
#include <dory/core/controllers/eventDispatcher.h>

namespace dory::core::controllers
{
    using namespace generic::registry;

    EventDispatcher::EventDispatcher(Registry& registry):
        DependencyResolver(registry)
    {}

    bool EventDispatcher::initialize(resources::IdType nodeId, resources::DataContext& context)
    {
        return true;
    }

    void EventDispatcher::stop(resources::IdType nodeId, resources::DataContext& context)
    {
    }

    void EventDispatcher::update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        _registry.get<
                    Service<events::window::Bundle::IDispatcher>,
                    Service<events::io::Bundle::IDispatcher>,
                    Service<events::filesystem::Bundle::IDispatcher>,
                    Service<events::scene::Bundle::IDispatcher>>(
                [&context](events::window::Bundle::IDispatcher* windowDispatcher,
                            events::io::Bundle::IDispatcher* ioDispatcher,
                            events::filesystem::Bundle::IDispatcher* fsDispatcher,
                           events::scene::Bundle::IDispatcher* sceneDispatcher)
            {
                windowDispatcher->fireAll(context);
                ioDispatcher->fireAll(context);
                fsDispatcher->fireAll(context);
                sceneDispatcher->fireAll(context);
            });
    }
}
