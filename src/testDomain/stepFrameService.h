#pragma once

namespace test
{
    class DORY_API StepFrameService: public dory::FrameService
    {
        private:
            bool isStop;
            int frameCounter;

        public:

        StepFrameService();

        void startLoop(dory::domain::Engine& engine, dory::DataContext& context);
        
        void endLoop();
    };
}