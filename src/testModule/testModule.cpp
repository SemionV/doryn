#include "testModule.h"

namespace testModule
{
    void TestModule::run(client::Registry& registry)
    {
        std::cout << "Run TestModule\n";
    }

    TestModule::~TestModule()
    {
        std::cout << "Unload of TestModule\n";
    }
}