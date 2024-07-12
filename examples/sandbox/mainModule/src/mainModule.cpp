#include <dory/sandbox/mainModule.h>

namespace dory::sandbox
{
    MainModule::MainModule():
            registry { configuration }
    {}

    int MainModule::run(bootstrap::StartupModuleContext& moduleContext) {
        std::cout << "Hello world!\n";
        return 0;
    }
}