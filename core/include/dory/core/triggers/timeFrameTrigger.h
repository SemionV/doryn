#pragma once

#include <optional>
#include <dory/core/iTrigger.h>
#include <dory/core/dependencyResolver.h>
#include <dory/core/resources/duration.h>

namespace dory::core::triggers
{
    class TimeFrameTrigger: public ITrigger, public DependencyResolver
    {
    private:
        std::chrono::nanoseconds timeAccumulator {};
        std::chrono::nanoseconds fixedDeltaTime {};

    public:
        resources::Duration duration;
        std::size_t maxUpdatesPerFrame { 5 };

        explicit TimeFrameTrigger(Registry& registry);

        resources::entities::NodeUpdateCounter check(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) final;
        bool initialize(resources::IdType nodeId, resources::DataContext& context) final;
    };
}
