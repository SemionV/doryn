#include <iostream>

#include "../base/engine.h"

using namespace std;

class TestController: public dory::Controller
{
    void initialize(const dory::DataContext& context) override
    {
    }

    void stop(const dory::DataContext& context) override
    {
    }

    void update(const std::chrono::microseconds timeStep, dory::DataContext& context) override
    {
    }
};

int main()
{
    cout << "doryn test application" << endl;

    dory::Engine engine;
    TestController controller;

    engine.addController(&controller);

    return 0;
}