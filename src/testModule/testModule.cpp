#include "testModule.h"

namespace testModule
{
    void TestModule::load(const dory::ModuleHandle& moduleHandle, client::Registry& registry)
    {
        std::cout << "Load TestModule\n";
    }

    TestModule::~TestModule()
    {
        std::cout << "Unload TestModule\n";
    }
}