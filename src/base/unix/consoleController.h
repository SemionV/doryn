#pragma once

#include "base/domain/controller.h"
#include "base/concurrency/individualProcessThread.h"
#include "base/domain/events/eventHub.h"
#include "base/domain/events/systemConsoleEventHub.h"

namespace dory::unix
{
    template<typename TDataContext>
    class ConsoleControllerFactory;

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
            processThread.invokeTask(concurrency::allocateActionTask([this]()
            {
                initscr();
                cbreak();
                keypad(stdscr, TRUE);
                noecho();
            }));

            std::cout << "SystemConsole.connect()" << std::endl;

            auto readInputTask = concurrency::allocateActionTask([this]()
            {
                int inputKey = getch();
                onKeyPressed(inputKey);
            });
            processThread.setRegularTask(readInputTask);

            processThread.run();

            return true;
        };

        void stop(domain::entity::IdType referenceId, TDataContext& context) override
        {
            processThread.stop();
            processThread.invokeTask(concurrency::allocateActionTask([this]()
            {
                endwin();
            }));
        };

        void update(dory::domain::entity::IdType referenceId, const domain::TimeSpan& timeStep, TDataContext& context) override
        {
            consoleEventHub.submit(context);
        }

    protected:
        void onKeyPressed(int key)
        {
            std::cout << std::this_thread::get_id() << ": add key pressed message: " << key << std::endl;

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