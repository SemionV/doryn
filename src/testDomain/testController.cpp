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
            if(message.keyPressed == 27)
            {
                context.isStop = true;
                std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
            }
            else if(message.keyPressed != 0)
            {
                std::cout << std::this_thread::get_id() << ": key pressed: " << message.keyPressed << std::endl;
            }
            else
            {
                std::cout << std::this_thread::get_id() << ": click: " << message.clickX << ", " << message.clickY << std::endl;
            }
        });

        messagePool.clean();
    }
}