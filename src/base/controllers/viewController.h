#pragma once

#include "base/controller.h"
#include "base/domain/view.h"

namespace dory
{
    class ViewController: public Controller
    {
        private:
            std::shared_ptr<View> view;

        public:
            ViewController(std::shared_ptr<View> view);

            void initialize(dory::DataContext& context) override;
            void stop(dory::DataContext& context) override;
            void update(const dory::TimeSpan& timeStep, dory::DataContext& context) override;
    };
}