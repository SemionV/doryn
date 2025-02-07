#pragma once

#include "dory/generic/baseTypes.h"
#include "dory/generic/model.h"
#include "resources/dataContext.h"
#include "resources/entities/pipelineNode.h"

namespace dory::core
{
    class IController: public generic::Interface
    {
    public:
        virtual bool initialize(resources::IdType nodeId, resources::DataContext& context) = 0;
        virtual void stop(resources::IdType nodeId, resources::DataContext& context) = 0;
        virtual void update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) = 0;
    };
}
