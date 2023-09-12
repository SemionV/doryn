#pragma once

#include "base/controller.h"
#include "base/configuration/configuration.h"
#include "base/domain/view.h"

namespace dory
{
    class ViewController: public Controller
    {
        protected:
            std::shared_ptr<View> view;
            std::shared_ptr<IConfiguration> configuration;

        public:
            ViewController(std::shared_ptr<IConfiguration> configuration, std::shared_ptr<View> view);

            bool initialize(dory::DataContext& context) override;
            void stop(dory::DataContext& context) override;
            void update(const dory::TimeSpan& timeStep, dory::DataContext& context) override;
    };
}