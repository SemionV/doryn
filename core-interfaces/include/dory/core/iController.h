#pragma once

#include "dory/generic/baseTypes.h"
#include "dory/generic/model.h"
#include "resources/entity.h"
#include "resources/dataContext.h"

namespace dory::core
{
    class IController: public generic::Interface
    {
    public:
        virtual bool initialize(resources::IdType nodeId, resources::DataContext& context) = 0;
        virtual void stop(resources::IdType nodeId, resources::DataContext& context) = 0;
        virtual void update(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) = 0;
    };

    class ITrigger: public generic::Interface
    {
    public:
        virtual bool check(resources::IdType nodeId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) = 0;
    };
}
