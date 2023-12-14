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
        std::shared_ptr<RepositoryReader<entity::View>> viewRepository;
        const TServiceLocator& serviceLocator;

    public:
        ViewController(const TServiceLocator& serviceLocator,
                std::shared_ptr<RepositoryReader<entity::View>> viewRepository):
            viewRepository(viewRepository),
            serviceLocator(serviceLocator)
        {}
    };
}