#include "dependencies.h"
#include "windowControllerParallel.h"

namespace dory::win32
{
    bool WindowControllerParallel::initialize(domain::entity::IdType referenceId, domain::DataContext& context)
    {
        auto pumpMessagesTask = multithreading::allocateActionTask([this]() 
        {
            pumpSystemMessages();

            const std::chrono::milliseconds threadMainSleepInterval = std::chrono::milliseconds(10);
            std::this_thread::sleep_for(threadMainSleepInterval);
        });

        windowsThread->setRegularTask(pumpMessagesTask);

        return true;
    }

    void WindowControllerParallel::stop(domain::entity::IdType referenceId, domain::DataContext& context)
    {

    }

    void WindowControllerParallel::update(domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, domain::DataContext& context)
    {
        submitEvents(context);
    }
}