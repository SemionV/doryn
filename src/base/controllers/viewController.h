#pragma once

#include "base/controller.h"
#include "base/configuration/configuration.h"
#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"

namespace dory
{
    template<class TWindow>
    class ViewController: public Controller
    {
        protected:
            int viewId;
            std::shared_ptr<EntityAccessor<View>> viewRepository;
            std::shared_ptr<IConfiguration> configuration;

        public:
            ViewController(int viewId, std::shared_ptr<EntityAccessor<View>> viewRepository, std::shared_ptr<IConfiguration> configuration):
                viewId(viewId),
                viewRepository(viewRepository),
                configuration(configuration)
            {        
            }
    };
}