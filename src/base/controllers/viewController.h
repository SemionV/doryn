#pragma once

#include "base/controller.h"
#include "base/configuration/configuration.h"
#include "base/domain/view.h"

namespace dory
{
    template<class TWindow>
    class ViewController: public Controller
    {
        protected:
            std::shared_ptr<View<TWindow>> view;
            std::shared_ptr<IConfiguration> configuration;

        public:
            ViewController(std::shared_ptr<IConfiguration> configuration, std::shared_ptr<View<TWindow>> view):
                configuration(configuration),
                view(view)
            {        
            }
    };
}