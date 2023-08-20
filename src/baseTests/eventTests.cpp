#include "dependencies.h"

TEST_CASE( "Emit event", "[events]" ) {
    bool handle1 = false;
    bool handle2 = false;
    int ap = 3, bp = 4;

    dory::EventDispatcher<int, int> event;

    event.attachHandler([&](int a, int b)
    {
        REQUIRE(a == ap);
        REQUIRE(b == bp);
        handle1 = true;
    });

    event.attachHandler([&](int a, int b)
    {
        REQUIRE(a == ap);
        REQUIRE(b == bp);
        handle2 = true;
    });

    event(ap, bp);

    REQUIRE(handle1);
    REQUIRE(handle2);
}

TEST_CASE( "Detach event handler", "[events]" ) {
    bool handle = false;

    dory::EventDispatcher<int, int> event;

    auto handler = event.attachHandler([&](int a, int b)
    {
        handle = true;
    });

    event.detachHandler(handler);

    event(3, 4);

    REQUIRE(!handle);
}

class TestClass
{
    public:
        void eventHandler(bool& handle)
        {
            handle = true;
        }
};

TEST_CASE( "Member function as handler", "[events]" ) {
    bool handle = false;

    dory::EventDispatcher<bool&> event;

    TestClass testObject;
    std::function<void(bool&)> f = std::bind(&TestClass::eventHandler, &testObject, std::placeholders::_1);
    event.attachHandler(f);

    event(handle);

    REQUIRE(handle);
}