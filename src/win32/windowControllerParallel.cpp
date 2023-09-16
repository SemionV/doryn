#include "dependencies.h"
#include "windowControllerParallel.h"

namespace dory::win32
{
    bool WindowControllerParallel::initialize(DataContext& context)
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

    void WindowControllerParallel::stop(DataContext& context)
    {

    }

    void WindowControllerParallel::update(const int referenceId, const TimeSpan& timeStep, DataContext& context)
    {
        submitEvents(context);
    }
}