#include "dependencies.h"

using namespace std;

int main()
{
    cout << "dory:native test application" << endl;

    dory::DataContext context;
    dory::Engine engine(context);
    
    test::TestController controller;
    engine.addController(&controller);

    dory::MessagePool inputMessagePool;
    dory::SystemConsole systemConsole;
    dory::InputController inputController(inputMessagePool);
    inputController.addDevice(&systemConsole);
    engine.addController(&inputController);

    engine.initialize(context);

    test::StepFrameService frameService;
    frameService.startLoop(engine);

    return 0;
}