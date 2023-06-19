#include <iostream>

#include "../base/frameService.h"
#include "../base/engine.h"

using namespace std;

class TestController: public dory::Controller
{
    private:
        int counter;
        const int frameCount;
        long timeSteps[60];

    public:

    TestController():
        counter(0),
        frameCount(60)
    {

    }

    void initialize(dory::DataContext& context) override
    {
    }

    void stop(dory::DataContext& context) override
    {
    }

    void update(const dory::TimeStep& timeStep, dory::DataContext& context) override
    {
        timeSteps[counter] = timeStep.duration;

        if(counter >= frameCount)
        {
            context.isStop = true;

            for(int i = 0; i < frameCount; i++)
            {
                cout << "timeStep: " << timeSteps[i] << " ns" << endl;
            }
        }

        counter++;
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
        dory::TimeStep timeStep;

        std::chrono::steady_clock::time_point lastTimestamp = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point currentTimestamp;
        std::chrono::nanoseconds duration;

        while(!isStop)
        {
            currentTimestamp = std::chrono::steady_clock::now();
            duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTimestamp - lastTimestamp);

            timeStep.duration = duration.count();

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

    FrameService frameService;
    frameService.startLoop(engine);

    return 0;
}