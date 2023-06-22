#ifndef STEP_FRAME_SERVICE_H_INC_DETECTOR
#define STEP_FRAME_SERVICE_H_INC_DETECTOR

#include <iostream>
#include <conio.h>

#include "../base/doryExport.h"
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

#endif