#include "testModule.h"

namespace testModule
{
    void TestModule::initialize(std::shared_ptr<dory::ILibrary> library, client::Registry& registry)
    {
        std::cout << "Load TestModule\n";
    }

    TestModule::~TestModule()
    {
        std::cout << "Unload TestModule\n";
    }
}