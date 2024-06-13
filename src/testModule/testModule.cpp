#include "testModule.h"

namespace testModule
{
    void TestModule::initialize(client::Registry &registry)
    {
        std::cout << "Initialization of TestModule\n";
    }
}