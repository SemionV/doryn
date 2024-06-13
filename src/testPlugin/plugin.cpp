#include "plugin.h"

namespace testPlugin
{
    void TestPlugin::initialize(client::Registry &registry)
    {
        std::cout << "Initialization of TestPlugin\n";
    }
}