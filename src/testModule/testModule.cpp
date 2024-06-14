#include "testModule.h"

namespace testModule
{
    void TestModule::run(client::Registry& registry)
    {
        std::cout << "Initialization of TestModule\n";
    }

    TestModule::~TestModule()
    {
        std::cout << "unload test module\n";
    }
}