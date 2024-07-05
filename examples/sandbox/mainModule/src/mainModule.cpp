#include <dory/sandbox/mainModule.h>

namespace dory::sandbox
{
    int MainModule::run(bootstrapper::StartupModuleContext &moduleContext) {
        std::cout << "Hello world!\n";
        return 0;
    }
}