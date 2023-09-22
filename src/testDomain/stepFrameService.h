#pragma once

namespace test
{
    class DORY_API StepFrameService: public dory::domain::services::IFrameService
    {
        private:
            bool isStop;
            int frameCounter;

        public:

        StepFrameService();

        void startLoop(dory::domain::Engine& engine, dory::domain::DataContext& context);
        
        void endLoop();
    };
}