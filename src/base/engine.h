#ifndef BASE_ENGINE_H_INC_DETECTOR
#define BASE_ENGINE_H_INC_DETECTOR
#include <vector>

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
            bool update(const TimeSpan& timeStep);
    };
}

#endif