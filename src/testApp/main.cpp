#include <iostream>

#include "../base/frameService.h"
#include "../base/engine.h"

using namespace std;

class TestController: public dory::Controller
{
    void initialize(dory::DataContext& context) override
    {
    }

    void stop(dory::DataContext& context) override
    {
    }

    void update(const std::chrono::microseconds timeStep, dory::DataContext& context) override
    {
        cout << "test controller update" << endl;
        cout << "timeStep: " << timeStep.count() << " microseconds" << endl;

        context.isStop = true;
    }
};

class FrameService: public dory::FrameService
{
    private:
        bool isStop;

    public:

    void startLoop(dory::Engine& engine) override
    {
        isStop = false;

        std::chrono::steady_clock::time_point lastTimestamp = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point currentTimestamp;

        while(!isStop)
        {
            currentTimestamp = std::chrono::steady_clock::now();
            std::chrono::microseconds timeStep = std::chrono::duration_cast<std::chrono::microseconds>(currentTimestamp - lastTimestamp);

            isStop = engine.update(timeStep);

            lastTimestamp = currentTimestamp;
        }
    }
    
    void endLoop() override
    {
        isStop = true;
    }
};

int main()
{
    cout << "doryn test application" << endl;

    dory::DataContext context;
    dory::Engine engine(context);
    TestController controller;
    engine.addController(&controller);

    //engine.update(std::chrono::microseconds::zero());

    FrameService frameService;
    frameService.startLoop(engine);

    return 0;
}