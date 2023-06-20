#pragma once

#include "../base/frameService.h"

namespace test
{
    class DORY_API StepFrameService: public dory::FrameService
    {
        private:
            bool isStop;
            int frameCounter;

        public:

        StepFrameService();

        void startLoop(dory::Engine& engine);
        
        void endLoop();
    };
}