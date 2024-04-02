#pragma once

#include "base/unix/dependencies.h"
#include "base/domain/controller.h"
#include "base/concurrency/individualProcessThread.h"
#include "base/domain/events/eventHub.h"
#include "base/domain/events/systemConsoleEventHub.h"

namespace dory::nunix
{
    template<typename TDataContext>
    class ConsoleControllerFactory;

    static struct termios oldt, currentt;

    template<class TDataContext>
    class ConsoleController: public domain::Controller<TDataContext>
    {
    private:
        concurrency::IndividualProcessThread processThread;

        using EventHubDispatcherType = domain::events::SystemConsoleEventHubDispatcher<TDataContext>;
        EventHubDispatcherType& consoleEventHub;

    public:
        using FactoryType = ConsoleControllerFactory<TDataContext>;

        explicit ConsoleController(EventHubDispatcherType& consoleEventHub):
                consoleEventHub(consoleEventHub)
        {}

        bool initialize(domain::entity::IdType referenceId, TDataContext& context) override
        {
            //see https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
            tcgetattr(STDIN_FILENO, &oldt);
            currentt = oldt;
            currentt.c_lflag &= ~ICANON; /* disable buffered i/o */
            currentt.c_lflag &= ~ECHO; /* set no echo mode */
            tcsetattr(STDIN_FILENO, TCSANOW, &currentt);

            auto readInputTask = concurrency::allocateActionTask([this]()
            {
                int inputKey = getchar();
                onKeyPressed(inputKey);
            });
            processThread.setRegularTask(readInputTask);

            processThread.run();

            return true;
        };

        void stop(domain::entity::IdType referenceId, TDataContext& context) override
        {
            tcsetattr(0, TCSANOW, &oldt);
            processThread.stop();
        };

        void update(dory::domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
        {
            consoleEventHub.submit(context);
        }

    protected:
        void onKeyPressed(int key)
        {
            domain::events::KeyPressedEventData eventData(key);
            consoleEventHub.addCase(std::forward<domain::events::KeyPressedEventData>(eventData));
        }
    };

    template<typename TDataContext>
    class ConsoleControllerFactory: public IServiceFactory<ConsoleControllerFactory<TDataContext>>
    {
    private:
        using ControllerInterfaceType = domain::Controller<TDataContext>;

        using EventHubDispatcherType = domain::events::SystemConsoleEventHubDispatcher<TDataContext>;
        EventHubDispatcherType& consoleEventHub;

    public:
        explicit ConsoleControllerFactory(EventHubDispatcherType& consoleEventHub):
                consoleEventHub(consoleEventHub)
        {}

        std::shared_ptr<ControllerInterfaceType> createInstanceImpl()
        {
            return std::static_pointer_cast<ControllerInterfaceType>(std::make_shared<ConsoleController<TDataContext>>(consoleEventHub));
        }
    };
}