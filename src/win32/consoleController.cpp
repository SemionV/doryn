#include "dependencies.h"
#include "consoleController.h"

namespace dory::win32
{
    bool ConsoleController::initialize(domain::entity::IdType referenceId, DataContext& context)
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

    void ConsoleController::stop(domain::entity::IdType referenceId, DataContext& context)
    {
        processThread.stop();
    }

    void ConsoleController::update(dory::domain::entity::IdType referenceId, const TimeSpan& timeStep, DataContext& context)
    {
        eventHub->submit(context);
    }

    void ConsoleController::onKeyPressed(int key)
    {
        std::cout << std::this_thread::get_id() << ": add key pressed message: " << key << std::endl;

        events::KeyPressedEventData eventData(key);
        eventHub->addCase(std::forward<events::KeyPressedEventData>(eventData));
    }

    void ConsoleController::bindStdHandlesToConsole()
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