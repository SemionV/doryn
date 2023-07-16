#include "dependencies.h"
#include "testController.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

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
        messagePool.iterate([&](std::shared_ptr<dory::Message> message)
        {
            if(message->messageType == dory::MessageType::ConsoleTestMessage)
            {
                std::shared_ptr<dory::ConsoleMessage> consoleMessage = std::static_pointer_cast<dory::ConsoleMessage>(message);

                if(consoleMessage->keyPressed == 27)
                {
                    context.isStop = true;
                    std::cout << std::this_thread::get_id() << ": ESC" << std::endl;
                }
                else if(consoleMessage->keyPressed != 0)
                {
                    std::cout << std::this_thread::get_id() << ": key pressed: " << consoleMessage->keyPressed << std::endl;
                }
                else
                {
                    std::cout << std::this_thread::get_id() << ": click: " << consoleMessage->clickX << ", " << consoleMessage->clickY << std::endl;
                }
            }
            else if(message->messageType == dory::MessageType::MouseTestMessage)
            {
                /*char DllPath[MAX_PATH] = {0};
                GetModuleFileName((HINSTANCE)&__ImageBase, _T(DllPath), _countof(DllPath));
                std::string path(DllPath);
                std::string base_filename = path.substr(path.find_last_of("/\\") + 1);*/

                std::shared_ptr<dory::WindowMessage> windowMessage = std::static_pointer_cast<dory::WindowMessage>(message);
                std::cout /*<< base_filename*/ << "(" << std::this_thread::get_id() << "): click: " << windowMessage->x << ", " << windowMessage->y << std::endl;
            }
        });

        messagePool.clean();
    }
}