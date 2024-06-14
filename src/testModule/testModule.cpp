#include "testModule.h"

namespace testModule
{
    void TestModule::run(dory::ModuleStateType moduleState, client::Registry& registry)
    {
        std::cout << "Run TestModule\n";
    }

    TestModule::~TestModule()
    {
        std::cout << "Unload TestModule\n";
    }
}