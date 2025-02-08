#pragma once

#include <dory/core/iController.h>
#include <dory/core/dependencyResolver.h>
#include <dory/core/resources/duration.h>

namespace dory::core::controllers
{
    class FrameCounter: public IController, public DependencyResolver
    {
    private:
        std::chrono::nanoseconds timeAccumulator {};
        std::chrono::nanoseconds fixedInterval {};
        std::size_t frameCount {};

    public:
        resources::Duration interval;

        explicit FrameCounter(core::Registry& registry);

        bool initialize(resources::IdType nodeId, resources::DataContext& context) final;
        void stop(resources::IdType nodeId, resources::DataContext& context) final;
        void update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) final;
    };
}