#pragma once

#include "controller.h"
#include "repositories/resourceScopeRepository.h"

namespace dory
{
    class DORY_API Engine
    {
        private:
            DataContext& dataContext;
            std::vector<Controller*> controllers;
            ResourceScope updateControllersScope;
            std::shared_ptr<ResourceScopeRepository> resourceScopeRepository;

        public:
            Engine(DataContext& context, std::shared_ptr<ResourceScopeRepository> resourceScopeRepository);
            ~Engine();
            void addController(Controller* controller);
            bool update(const TimeSpan& timeStep);
            void initialize(DataContext& context);
    };
}