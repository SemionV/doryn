#pragma once

#include <vector>

#include "doryExport.h"
#include "controller.h"

namespace dory
{
    class DORY_API Engine
    {
        private:
            DataContext& dataContext;
            std::vector<Controller*> controllers;      

        public:
            Engine(DataContext& context);
            ~Engine();
            void addController(Controller* controller);
            bool update(const std::chrono::microseconds timeStep);
    };
}