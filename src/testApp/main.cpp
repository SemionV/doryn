#include <iostream>

#include "../base/frameService.h"
#include "../base/basicFrameService.h"
#include "../base/engine.h"
#include "../testDomain/testController.h"

using namespace std;

int main()
{
    cout << "dory:native test application" << endl;

    dory::DataContext context;
    dory::Engine engine(context);
    test::TestController controller;
    engine.addController(&controller);

    dory::BasicFrameService frameService;
    frameService.startLoop(engine);

    return 0;
}