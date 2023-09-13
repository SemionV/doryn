#pragma once

#include "controller.h"

namespace dory
{
    class DORY_API Engine
    {
        private:
            DataContext& dataContext;
            std::vector<std::shared_ptr<Controller>> controllers;

        public:
            Engine(DataContext& context);
            ~Engine();
            void addController(std::shared_ptr<Controller> controller);
            void removeController(std::shared_ptr<Controller> controller);
            bool update(const TimeSpan& timeStep);
            void initialize(DataContext& context);
    };
}