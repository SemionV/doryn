#include "dory/game/engine/registryFactory.h"
#include <iostream>

#ifdef DORY_MAIN_FUNCTION_UNIX
int main()
#endif
#ifdef DORY_MAIN_FUNCTION_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
#endif
{
    dory::game::engine::RegistryFactory factory;

    auto registry = factory.createRegistry();
    std::cout << registry->services.fileService->getMessage() << "\n";

    return 0;
}