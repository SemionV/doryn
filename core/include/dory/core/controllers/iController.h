#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/generic/model.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::controllers
{
    class IController: public generic::Interface
    {
    public:
        virtual bool initialize(resources::IdType referenceId, resources::DataContext& context) = 0;
        virtual void stop(resources::IdType referenceId, resources::DataContext& context) = 0;
        virtual void update(resources::IdType referenceId, const generic::model::TimeSpan& timeStep, resources::DataContext& context) = 0;
    };
}
