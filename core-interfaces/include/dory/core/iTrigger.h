#pragma once

#include "dory/generic/baseTypes.h"
#include "dory/generic/model.h"
#include "resources/dataContext.h"
#include "resources/entities/pipelineNode.h"

namespace dory::core
{
    class ITrigger: public generic::Interface
    {
    public:
        virtual resources::entities::NodeUpdateCounter check(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) = 0;
    };
}
