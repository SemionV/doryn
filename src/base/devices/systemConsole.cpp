#include "base/dependencies.h"
#include "systemConsole.h"

namespace dory
{
    SystemConsole::SystemConsole():
    inputKey(0),
    isStop(false)
    {        
    }

    bool SystemConsole::connect()
    {
        isStop = false;

        std::cout << "SystemConsole.connect()" << std::endl;

        workingThread = std::thread(&monitorSystemConsole, this);
        workingThread.detach();

        return true;
    }

    void SystemConsole::monitorSystemConsole()
    {
        while(!isStop)
        {
            inputKey = getch();//consider thread safety of this operation
        }
    }

    void SystemConsole::disconnect()
    {
        isStop = true;
    }
    
    void SystemConsole::readUpdates(MessagePool& messagePool)
    {
        if(inputKey)
        {
            ConsoleMessage message(inputKey);
            messagePool.addMessage(message);
            inputKey = 0;
        }
    }
}