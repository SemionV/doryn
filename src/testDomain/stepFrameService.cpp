#include "dependencies.h"
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
        char key;

#ifdef __unix__
        initscr();
        cbreak();
        keypad(stdscr, TRUE);
        noecho();
#endif

        std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

        while(!isStop)
        {
            key = getch();

            if(key == 27)
            {
                isStop = true;
            }
            else
            {
                timeStep.duration = 16666;
                isStop = engine.update(timeStep);
            }

            frameCounter++;
        }

#ifdef __unix__
        endwin();
#endif
    }
    
    void StepFrameService::endLoop()
    {
        isStop = true;
    }
}