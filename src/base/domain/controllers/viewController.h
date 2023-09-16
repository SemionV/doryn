#pragma once

#include "base/domain/controller.h"
#include "base/configuration/configuration.h"
#include "base/entityRepository.h"
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