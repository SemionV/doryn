#pragma once

#include "base/domain/controller.h"
#include "base/configuration/configuration.h"
#include "base/entityRepository.h"
#include "base/domain/entity.h"

namespace dory::domain
{
    template<class TWindow>
    class ViewController: public Controller
    {
        protected:
            entity::IdType viewId;
            std::shared_ptr<RepositoryReader<entity::View>> viewRepository;
            std::shared_ptr<IConfiguration> configuration;

        public:
            ViewController(entity::IdType viewId, std::shared_ptr<RepositoryReader<entity::View>> viewRepository, std::shared_ptr<IConfiguration> configuration):
                viewId(viewId),
                viewRepository(viewRepository),
                configuration(configuration)
            {        
            }
    };
}