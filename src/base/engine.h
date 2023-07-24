#pragma once

#include "controller.h"
#include "repositories/resourceScopeDispatcher.h"

namespace dory
{
    class DORY_API Engine
    {
        private:
            DataContext& dataContext;
            std::vector<Controller*> controllers;
            ResourceScope updateControllersScope;
            std::shared_ptr<ResourceScopeDispatcher> resourceScopeDispatcher;

        public:
            Engine(DataContext& context, std::shared_ptr<ResourceScopeDispatcher> resourceScopeDispatcher, ResourceScope updateControllersScope);
            ~Engine();
            void addController(Controller* controller);
            bool update(const TimeSpan& timeStep);
            void initialize(DataContext& context);
    };
}