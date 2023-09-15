#include "dependencies.h"
#include "win32WindowControllerParallel.h"

namespace dory::win32
{
    bool Win32WindowControllerParallel::initialize(DataContext& context)
    {
        auto pumpMessagesTask = dory::allocateActionTask([this]() 
        {
            pumpSystemMessages();

            const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(10);
            std::this_thread::sleep_for(threadMainSleepInterval);
        });

        windowsThread->setRegularTask(pumpMessagesTask);

        return true;
    }

    void Win32WindowControllerParallel::stop(DataContext& context)
    {

    }

    void Win32WindowControllerParallel::update(const TimeSpan& timeStep, DataContext& context)
    {
        submitEvents(context);
    }

    /*std::shared_ptr<Win32Window> WindowSystemParallel::createWindow(const WindowParameters& parameters)
    {
        auto createWindowTask = dory::allocateFunctionTask<std::shared_ptr<Win32Window>>([this](const WindowParameters& parameters) 
        {
            return WindowSystem::createWindow(parameters);
        }, parameters);

        processThread.invokeTask(createWindowTask);

        return createWindowTask->getResult();
    }*/
}