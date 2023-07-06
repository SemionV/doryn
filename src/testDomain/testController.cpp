#include "dependencies.h"
#include "testController.h"

namespace test
{
    TestController::TestController(dory::MessagePool& messagePool):
            counter(0),
            messagePool(messagePool)
    {
    }

    void TestController::initialize(dory::DataContext& context)
    {
    }

    void TestController::stop(dory::DataContext& context)
    {
    }

    void TestController::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        messagePool.iterate([&](dory::ConsoleMessage message)
        {
            std::cout << "TestController::key pressed: ";
            if(message.keyPressed == 27)
            {
                context.isStop = true;
                std::cout << "ESC";
            }
            else if(message.keyPressed != 0)
            {
                std::cout << message.keyPressed;
            }
            else
            {
                std::cout << "click: " << message.clickX << ", " << message.clickY;
            }

            std::cout << std::endl;
        });

        messagePool.clean();
    }
}