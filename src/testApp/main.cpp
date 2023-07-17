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
    auto windowSystem = std::make_shared<doryWindows::WindowSystemParallel>();
    auto windowSystemListener = std::make_shared<dory::DeviceListener>();
    windowSystem->attachListener(windowSystemListener);

    auto consoleSystem = std::make_shared<doryWindows::ConsoleSystem>();
    auto consoleSystemListener = std::make_shared<dory::DeviceListener>();
    consoleSystem->attachListener(consoleSystemListener);

    dory::InputController inputController(inputMessagePool);
    engine.addController(&inputController);

    inputController.addDevice(consoleSystem);
    inputController.addDeviceListener(consoleSystemListener);

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

namespace helper
{
    template <std::size_t... Ts>
    struct index 
    {
        enum { count = sizeof...(Ts) };
        std::size_t values[count] = {Ts...};
        const std::size_t valuesCount = count;
    };

    template <std::size_t N, std::size_t... Ts>
    struct gen_seq : gen_seq<N - 1, N - 1, Ts...> {};

    template <std::size_t... Ts>
    struct gen_seq<0, Ts...> : index<Ts...> {};
}
    


template <typename... Args>
void variadicFunction(Args...)
{
    auto index = helper::gen_seq<sizeof...(Args)>{};

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

    auto addFunction = dory::makeFunctionTask<int>([] (int a, int b) { int c = a + b; return c;}, a, b);
    addFunction();

    auto addAction = dory::makeActionTask([] (int a, int b) { int c = a + b; std::cout << c << std::endl;}, a, b);
    addAction();

    std::cout << "add action: " << addFunction.getResult() << std::endl;

    //variadicFunction(1, "value", 0.4f);

    return runDory();
}