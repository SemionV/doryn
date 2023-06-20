#pragma once

#include "frameService.h"

namespace dory
{
    class DORY_API BasicFrameService: public FrameService
    {
        private:
            bool isStop;

        public:

        void startLoop(Engine& engine);
        
        void endLoop();
    };
}