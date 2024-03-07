#pragma once

#include "base/domain/controller.h"
#include "base/configuration/configuration.h"
#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"

namespace dory::domain
{
    template<class TDataContext, typename TServiceLocator>
    class ViewController: public Controller<TDataContext>
    {
    protected:
        TServiceLocator& serviceLocator;

    public:
        explicit ViewController(TServiceLocator& serviceLocator):
            serviceLocator(serviceLocator)
        {}
    };
}