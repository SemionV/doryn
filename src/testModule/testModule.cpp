#include "testModule.h"

namespace testModule
{
    void TestModule::run(const dory::ModuleHandle& moduleHandle, client::Registry& registry)
    {
        std::cout << "Run TestModule\n";
    }

    TestModule::~TestModule()
    {
        std::cout << "Unload TestModule\n";
    }
}