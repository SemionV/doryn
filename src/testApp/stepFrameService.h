#pragma once

namespace testApp
{
    template<class TDataContext>
    class DORY_API StepFrameService: public dory::domain::services::IFrameService<TDataContext>
    {
        private:
            bool isStop;
            int frameCounter;

        public:
            StepFrameService::StepFrameService():
                isStop(false),
                frameCounter(0)
            {}

            void startLoop(dory::domain::Engine<TDataContext>& engine, TDataContext& context)
            {
                isStop = false;
                dory::domain::TimeSpan timeStep(dory::domain::UnitScale::Micro);
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
                        isStop = engine.update(context, timeStep);
                    }

                    frameCounter++;
                }

#ifdef __unix__
                endwin();
#endif
            }
        
            void endLoop()
            {
                isStop = true;
            }
    };
}