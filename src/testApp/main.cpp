#include <iostream>

#include "../base/frameService.h"
//#include "../base/basicFrameService.h"
#include "../base/engine.h"
#include "../testDomain/testController.h"
#include "../testDomain/stepFrameService.h"

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