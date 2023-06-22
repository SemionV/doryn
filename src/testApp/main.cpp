#include "dependencies.ext.h"

using namespace std;

int main()
{
    cout << "dory:native test application" << endl;

    dory::DataContext context;
    dory::Engine engine(context);
    test::TestController controller;
    engine.addController(&controller);

    test::StepFrameService frameService;
    frameService.startLoop(engine);

    return 0;
}