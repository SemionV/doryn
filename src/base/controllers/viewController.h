#pragma once

#include "base/controller.h"
#include "base/configuration/configuration.h"
#include "base/domain/repositories/viewEntityRepository.h"

namespace dory
{
    template<class TWindow>
    class ViewController: public Controller
    {
        protected:
            int viewId;
            std::shared_ptr<ViewEntityRepository> viewRepository;
            std::shared_ptr<IConfiguration> configuration;

        public:
            ViewController(int viewId, std::shared_ptr<ViewEntityRepository> viewRepository, std::shared_ptr<IConfiguration> configuration):
                viewId(viewId),
                viewRepository(viewRepository),
                configuration(configuration)
            {        
            }
    };
}