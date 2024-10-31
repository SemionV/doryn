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
        virtual bool initialize(resources::IdType referenceId, resources::DataContext& context) = 0;
        virtual void stop(resources::IdType referenceId, resources::DataContext& context) = 0;
        virtual void update(resources::IdType referenceId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) = 0;
    };
}
