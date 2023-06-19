#pragma once

#include <vector>

#include "doryExport.h"
#include "controller.h"

namespace dory
{
    class DORY_API Engine
    {
        private:
            std::vector<Controller*> controllers;

        public:
            Engine();
            ~Engine();
            void addController(Controller* controller);
    };
}