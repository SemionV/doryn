#ifndef STEP_FRAME_SERVICE_H_INC_DETECTOR
#define STEP_FRAME_SERVICE_H_INC_DETECTOR

#include <iostream>

#include "../base/controller.h"
#include "../base/doryExport.h"

using namespace std;

namespace test
{
    class DORY_API TestController: public dory::Controller
    {
        private:
            int counter;


        public:

        TestController();

        void initialize(dory::DataContext& context);

        void stop(dory::DataContext& context);

        void update(const dory::TimeSpan& timeStep, dory::DataContext& context);
    };
}

#endif