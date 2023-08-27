#include "dependencies.h"
#include "testLogic.h"

namespace test
{
    TestLogic::TestLogic(std::shared_ptr<dory::SystemConsoleEventHub> consoleEventHub,
        std::shared_ptr<dory::SystemWindowEventHub> windowEventHub):
        consoleEventHub(consoleEventHub),
        windowEventHub(windowEventHub)
    {
        consoleEventHub->onKeyPressed() += [] (dory::DataContext& context, dory::KeyPressedEventData& keyPressedEventData)
        {
            if(keyPressedEventData.keyPressed == 27)
            {
                context.isStop = true;
                std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
            }
            else if(keyPressedEventData.keyPressed != 0)
            {
                std::cout << std::this_thread::get_id() << ": key pressed: " << keyPressedEventData.keyPressed << std::endl;
            }
        };

        windowEventHub->onMouseClick() += [] (dory::DataContext& context, dory::MouseClickEventData& mouseClickEventData)
        {
            std::cout << std::this_thread::get_id() << ": click: " << mouseClickEventData.x << ", " << mouseClickEventData.y << std::endl;
        };
    }
}