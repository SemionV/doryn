#include "testModule2.h"

namespace testModule
{
    void TestModule2::attach(dory::LibraryHandle library, client::Registry& registry)
    {
        std::cout << "Load TestModule 2\n";
    }

    TestModule2::~TestModule2()
    {
        std::cout << "Unload TestModule 2\n";
    }
}