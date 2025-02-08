#pragma once

#include <dory/core/services/iPipelineService.h>

namespace dory::core
{
    class Registry;
}

namespace dory::core::services
{
    class PipelineService: public IPipelineService
    {
    private:
        Registry& _registry;

    public:
        explicit PipelineService(Registry& registry);

        void update(resources::DataContext& context, const generic::model::TimeSpan& timeStep) final;
        void initialize(resources::DataContext& context) final;
        void stop(resources::DataContext& context) final;
        void buildPipeline(resources::scene::Scene& scene, const resources::scene::configuration::Pipeline& pipeline, resources::DataContext& context) final;
    };
}
