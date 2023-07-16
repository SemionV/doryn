#include "dependencies.h"
#include "windowSystemParallel.h"

namespace doryWindows
{
    bool WindowSystemParallel::connect()
    {
        auto pumpMessagesTask = std::make_shared<dory::LambdaTask>([this]() 
        {
            pumpSystemMessages();

            const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(10);
            std::this_thread::sleep_for(threadMainSleepInterval);
        });

        processThread = std::make_shared<dory::IndividualProcessThread>(pumpMessagesTask);
        processThread->run();

        return true;
    }

    void WindowSystemParallel::disconnect()
    {
        if(processThread)
        {
            processThread->stop();
        }
    }

    void WindowSystemParallel::update()
    {
    }

    class CreateWindowTask: public dory::TaskFunction<std::shared_ptr<dory::Window>, const WindowParameters&>
    {
        public:
            std::shared_ptr<dory::Window> invoke(const WindowParameters& windowParameters) override
            {
                return nullptr;
            }
    };

    std::shared_ptr<dory::Window> WindowSystemParallel::createWindow(const WindowParameters& parameters)
    {
        std::shared_ptr<dory::Window> window;
        auto createWindowTask = std::make_shared<dory::LambdaTask>([=]() 
        {
            std::cout << std::this_thread::get_id() << ": create a system window" << std::endl;
            //window = WindowSystem::createWindow(parameters);
            WindowSystem::createWindow(parameters);
        });

        processThread->invokeTask(createWindowTask);

        return window;
    }
}