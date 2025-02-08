#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/dataContext.h>
#include <dory/generic/model.h>

namespace dory::core::services
{
    class IPipelineService: public generic::Interface
    {
    public:
        virtual void update(resources::DataContext& context, const generic::model::TimeSpan& timeStep) = 0;
        virtual void initialize(resources::DataContext& context) = 0;
        virtual void stop(resources::DataContext& context) = 0;
        virtual void buildPipeline(resources::scene::Scene& scene, const resources::scene::configuration::Pipeline& pipeline, resources::DataContext& context) = 0;
    };
}
