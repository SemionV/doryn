#include "testModule.h"

namespace testModule
{
    void TestModule::attach(std::weak_ptr<dory::ILibrary> library, client::Registry& registry)
    {
        std::cout << "Load TestModule\n";
    }

    void TestModule::detach(client::Registry &registry)
    {
        std::cout << "Unload TestModule\n";
    }
}