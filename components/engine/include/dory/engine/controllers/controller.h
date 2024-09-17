#pragma once

#include "dory/engine/resources/dataContext.h"
#include "dory/engine/resources/types.h"
#include "dory/engine/resources/entity.h"

namespace dory::engine::controllers
{
    template<class TDataContext>
    class Controller
    {
    public:
        virtual ~Controller() = default;

        explicit Controller() = default;

        virtual bool initialize(resources::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void stop(resources::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void update(resources::entity::IdType referenceId, const resources::TimeSpan& timeStep, TDataContext& context) = 0;
    };
}