#include <dory/core/registry.h>
#include <dory/core/controllers/stateUpdater.h>

namespace dory::core::controllers
{
    StateUpdater::StateUpdater(Registry& registry):
        DependencyResolver(registry)
    {}

    bool StateUpdater::initialize(resources::IdType nodeId, resources::DataContext& context)
    {
        return true;
    }

    void StateUpdater::stop(resources::IdType nodeId, resources::DataContext& context)
    {
    }

    void StateUpdater::update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context)
    {
        if(auto viewService = _registry.get<services::IViewService>())
        {
            viewService->updateViewsState(context.viewStates);
            context.viewStatesUpdateTime = {};
            context.viewStatesUpdateTimeDelta = timeStep;
        }
    }
}
