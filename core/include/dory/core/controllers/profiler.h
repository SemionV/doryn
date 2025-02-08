#pragma once

#include <dory/core/iController.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::controllers
{
    class Profiler: public IController, public DependencyResolver
    {
    private:
        std::size_t frameCount {};

    public:

        explicit Profiler(Registry& registry);

        bool initialize(resources::IdType nodeId, resources::DataContext& context) final;
        void stop(resources::IdType nodeId, resources::DataContext& context) final;
        void update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) final;
    };
}
