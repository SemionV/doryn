#include <iostream>
#include "stepFrameService.h"

namespace test
{
    StepFrameService::StepFrameService():
        isStop(false),
        frameCounter(0)
    {

    }

    void StepFrameService::startLoop(dory::Engine& engine)
    {
        isStop = false;
        dory::TimeSpan timeStep(dory::UnitScale::Micro);
        char key(' ');

        std::cout << "Press any key to process next frame or ESC to exit" << std::endl;

        while(!isStop)
        {
            timeStep.duration = 16666;

            isStop = engine.update(timeStep);

            key = std::cin.get();
            if(key == 27)
            {
                isStop = true;
            }

            frameCounter++;
        }
    }
    
    void StepFrameService::endLoop()
    {
        isStop = true;
    }
}