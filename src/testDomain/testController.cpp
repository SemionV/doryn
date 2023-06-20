#include "testController.h"

namespace test
{
    TestController::TestController():
            counter(0)
    {
    }

    void TestController::initialize(dory::DataContext& context)
    {
    }

    void TestController::stop(dory::DataContext& context)
    {
    }

    void TestController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        timeSteps[counter] = timeStep;

        if(counter >= frameCount)
        {
            context.isStop = true;

            for(int i = 0; i < frameCount; i++)
            {
                cout << "timeStep: " << dory::TimeConverter::ToMilliseconds(timeSteps[i]) << " ms" << endl;
            }
        }

        counter++;
    }
}