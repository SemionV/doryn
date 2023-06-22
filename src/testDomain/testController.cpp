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
        cout << counter << ": timeStep: " << dory::TimeConverter::ToMilliseconds(timeStep) << " ms" << endl;

        counter++;
    }
}