#include "dependencies.h"

#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

int runDory()
{
    dory::MessagePool inputMessagePool;
    dory::DataContext context;
    dory::Engine engine(context);
    auto windowSystem = std::make_shared<doryWindows::WindowSystem>();
    auto windowSystemListener = std::make_shared<dory::DeviceListener>();
    windowSystem->attachListener(windowSystemListener);

    dory::InputController inputController(inputMessagePool);
    engine.addController(&inputController);

    dory::SystemConsole systemConsole;
    inputController.addDeviceListener(&systemConsole);

    inputController.addDevice(windowSystem);
    inputController.addDeviceListener(windowSystemListener);

    test::TestController controller(inputMessagePool);
    engine.addController(&controller);

    engine.initialize(context);

    std::cout << "dory:native test application" << std::endl;
    std::cout << "Press any key to process to render frame OR ESC to exit\r" << std::endl;

    const doryWindows::WindowParameters windowParameters;
    windowSystem->createWindow(windowParameters);

    dory::BasicFrameService frameService;
    frameService.startLoop(engine);

    std::cout << "Session is over." << std::endl;

    return 0;
}

/*int main()
{
    return runDory();
}*/

template<int n> struct funStruct
{
    enum { val = 2*funStruct<n-1>::val };
};
 
template<> struct funStruct<0>
{
    enum { val = 1 };
};

template<typename...>
using try_to_instantiate = void;

template<typename T, typename Attempt = void>
struct is_incrementable: std::false_type{};

template<typename T>
struct is_incrementable<T, try_to_instantiate<decltype(++std::declval<T&>())>>: std::true_type{};

template<bool Tv, typename Attempt = void>
struct is_true: std::false_type{};

template<>
struct is_true<true>: std::true_type{};

template <typename... Args>
void variadicFunction(Args...)
{
    auto index = dory::helper::gen_seq<sizeof...(Args)>{};

    for(std::size_t i = 0; i < index.valuesCount; i++)
    {
        std::cout << index.values[i] << std::endl;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    /*std::cout << funStruct<8>::val << std::endl;

    std::cout << is_incrementable<int>::value << std::endl;
    std::cout << is_incrementable<dory::Engine>::value << std::endl;*/

    int a = 2;
    int b = 3;

    auto add = dory::make_action<int>([] (int a, int b) { int c = a + b; return c;}, a, b);
    add.act();

    std::cout << "add action: " << add.result << std::endl;

    //variadicFunction(1, "value", 0.4f);

    return runDory();
}