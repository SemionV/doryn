#include "dependencies.h"

int main()
{
    std::cout << "dory:native test application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    dory::MessagePool inputMessagePool;
    dory::DataContext context;
    dory::Engine engine(context);
    
    test::TestController controller(inputMessagePool);
    engine.addController(&controller);

    dory::SystemConsole systemConsole;
    dory::InputController inputController(inputMessagePool);
    inputController.addDevice(&systemConsole);
    engine.addController(&inputController);

    engine.initialize(context);

    dory::BasicFrameService frameService;
    frameService.startLoop(engine);

    std::cout << "Session is over." << std::endl;

    return 0;
}