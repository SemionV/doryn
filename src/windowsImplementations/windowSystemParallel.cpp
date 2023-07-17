#include "dependencies.h"
#include "windowSystemParallel.h"

namespace doryWindows
{
    bool WindowSystemParallel::connect()
    {
        auto pumpMessagesTask = dory::allocateActionTask([this]() 
        {
            pumpSystemMessages();

            const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(10);
            std::this_thread::sleep_for(threadMainSleepInterval);
        });

        processThread.setRegularTask(pumpMessagesTask);

        processThread.run();

        return true;
    }

    void WindowSystemParallel::disconnect()
    {
        processThread.stop();
    }

    void WindowSystemParallel::update()
    {
    }

    std::shared_ptr<dory::Window> WindowSystemParallel::createWindow(const WindowParameters& parameters)
    {
        auto createWindowTask = dory::allocateFunctionTask<std::shared_ptr<dory::Window>>([this](const WindowParameters& parameters) 
        {
            return WindowSystem::createWindow(parameters);
        }, parameters);

        processThread.invokeTask(createWindowTask);

        return createWindowTask->getResult();
    }
}