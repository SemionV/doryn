#pragma once

namespace test
{
    class DORY_API TestController: public dory::Controller
    {
        private:
            int counter;
            dory::MessagePool& messagePool;

        public:

        TestController(dory::MessagePool& messagePool);

        void initialize(dory::DataContext& context);

        void stop(dory::DataContext& context);

        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);
    };
}