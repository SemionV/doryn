#pragma once

#include <optional>
#include <dory/core/iTrigger.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::triggers
{
    class TimeFrameTrigger: public ITrigger, public DependencyResolver
    {
    private:
        static constexpr int maxUpdatesPerFrame = 5;
        std::chrono::nanoseconds timeAccumulator {};

    public:
        std::size_t nanoseconds {};

        explicit TimeFrameTrigger(Registry& registry);

        resources::entities::NodeUpdateCounter check(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) final;
    };
}
