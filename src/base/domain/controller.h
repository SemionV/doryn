#pragma once

#include "dataContext.h"
#include "types.h"
#include "entity.h"

namespace dory::domain
{
    template<class TDataContext, typename TServiceLocator>
    class Controller: public Service<TServiceLocator>
    {
    public:
        explicit Controller(TServiceLocator& serviceLocator):
                Service<TServiceLocator>(serviceLocator)
        {}

        virtual ~Controller() = default;

        virtual bool initialize(domain::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void stop(domain::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void update(domain::entity::IdType referenceId, const TimeSpan& timeStep, TDataContext& context) = 0;
    };

    template<class TDataContext>
    class Controller2
    {
    public:
        virtual bool initialize(domain::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void stop(domain::entity::IdType referenceId, TDataContext& context) = 0;
        virtual void update(domain::entity::IdType referenceId, const TimeSpan& timeStep, TDataContext& context) = 0;
    };
}