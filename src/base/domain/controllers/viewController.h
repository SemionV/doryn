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
            std::shared_ptr<RepositoryReader<entity::View>> viewRepository;
            std::shared_ptr<configuration::IConfiguration> configuration;

        public:
            ViewController(std::shared_ptr<RepositoryReader<entity::View>> viewRepository, std::shared_ptr<configuration::IConfiguration> configuration):
                viewRepository(viewRepository),
                configuration(configuration)
            {}
    };
}