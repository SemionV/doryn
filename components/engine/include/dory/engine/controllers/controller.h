#pragma once

#include "dory/engine/resources/dataContext.h"
#include "dory/engine/resources/types.h"
#include "dory/engine/resources/entity.h"

namespace dory::domain
{
    template<class TDataContext>
    class Controller
    {
    public:
        virtual ~Controller() = default;

        explicit Controller() = default;

        virtual bool initialize(domain::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void stop(domain::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void update(domain::entity::IdType referenceId, const TimeSpan& timeStep, TDataContext& context) = 0;
    };
}