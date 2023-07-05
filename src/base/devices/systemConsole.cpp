#include "base/dependencies.h"
#include "systemConsole.h"

namespace dory
{
    bool SystemConsole::connect()
    {
        std::cout << "SystemConsole.connect()" << std::endl;

        return true;
    }

    void SystemConsole::disconnect()
    {

    }
    
    void SystemConsole::readUpdates(MessagePool& messagePool)
    {
        
    }
}