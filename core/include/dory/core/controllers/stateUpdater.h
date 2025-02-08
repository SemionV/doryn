#pragma once

#include <dory/core/iController.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::controllers
{
    class StateUpdater: public IController, public DependencyResolver
    {
    public:

        explicit StateUpdater(Registry& registry);

        bool initialize(resources::IdType nodeId, resources::DataContext& context) final;
        void stop(resources::IdType nodeId, resources::DataContext& context) final;
        void update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) final;
    };
}