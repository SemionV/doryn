#include "base/dependencies.h"
#include "systemConsole.h"

namespace dory
{
    SystemConsole::SystemConsole():
        inputKey(0)
    {
        auto readInputTask = std::make_shared<LambdaTask>([this]() 
        {  
            int inputKey = getch();
            this->onInput(inputKey);
        });

        individualThread = std::make_shared<IndividualProcessThread>(readInputTask);
    }

    bool SystemConsole::connect()
    {
        if(AllocConsole())
        {
            bindStdHandlesToConsole();
        }

        std::cout << "SystemConsole.connect()" << std::endl;

        individualThread->run();

        return true;
    }

    void SystemConsole::onInput(int key)
    {
        std::cout << std::this_thread::get_id() << ": add key pressed message: " << key << std::endl;
        inputKey = key;
    }

    void SystemConsole::bindStdHandlesToConsole()
    {
        //TODO: Add Error checking.
        
        // Redirect the CRT standard input, output, and error handles to the console
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stderr);
        freopen("CONOUT$", "w", stdout);
        
        // Note that there is no CONERR$ file
        HANDLE hStdout = CreateFileA("CONOUT$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        HANDLE hStdin = CreateFileA("CONIN$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        
        SetStdHandle(STD_OUTPUT_HANDLE,hStdout);
        SetStdHandle(STD_ERROR_HANDLE,hStdout);
        SetStdHandle(STD_INPUT_HANDLE,hStdin);

        //Clear the error state for each of the C++ standard stream objects. 
        std::wclog.clear();
        std::clog.clear();
        std::wcout.clear();
        std::cout.clear();
        std::wcerr.clear();
        std::cerr.clear();
        std::wcin.clear();
        std::cin.clear();
    }

    void SystemConsole::disconnect()
    {
        individualThread->stop();
    }

    void SystemConsole::handleMessage(std::shared_ptr<DeviceMessage> message)
    {

    }
    
    void SystemConsole::readUpdates(MessagePool& messagePool)
    {
        if(inputKey)
        {
            ConsoleMessage message(inputKey, -1, -1);
            messagePool.addMessage(message);
            inputKey = 0;
        }
    }
}