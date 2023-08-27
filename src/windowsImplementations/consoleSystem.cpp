#include "dependencies.h"
#include "consoleSystem.h"

namespace doryWindows
{
    bool ConsoleSystem::connect()
    {
        if(AllocConsole())
        {
            bindStdHandlesToConsole();
        }

        std::cout << "SystemConsole.connect()" << std::endl;

        auto readInputTask = dory::allocateActionTask([this]() 
        {  
            int inputKey = getch();
            onKeyPressed(inputKey);
        });
        processThread.setRegularTask(readInputTask);

        processThread.run();

        return true;
    }

    void ConsoleSystem::disconnect()
    {
        processThread.stop();
    }

    void ConsoleSystem::update()
    {
    }

    void ConsoleSystem::onKeyPressed(int key)
    {
        std::cout << std::this_thread::get_id() << ": add key pressed message: " << key << std::endl;

        /*auto message = std::make_shared<dory::ConsoleMessage>(key);
        propagateMessage(message);*/
        dory::KeyPressedEventData eventData(key);
        eventHub->addCase(std::forward<dory::KeyPressedEventData>(eventData));
    }

    void ConsoleSystem::bindStdHandlesToConsole()
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
}