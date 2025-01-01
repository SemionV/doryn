#pragma once

#include <dory/core/iController.h>
#include <dory/core/dependencyResolver.h>
#include <dory/core/resources/scene/enttScene.h>

namespace dory::core::controllers
{
    class AccelerationMovementController: public core::IController, public core::DependencyResolver
    {
    public:
        explicit AccelerationMovementController(core::Registry& registry);

        bool initialize(core::resources::IdType referenceId, core::resources::DataContext& context) override;
        void stop(core::resources::IdType referenceId, core::resources::DataContext& context) override;
        void update(core::resources::IdType referenceId, const generic::model::TimeSpan& timeStep, core::resources::DataContext& context) override;
    };
}