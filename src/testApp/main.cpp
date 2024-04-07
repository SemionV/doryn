#include "dependencies.h"
#include "project.h"

int run()
{
    auto project = testApp::Project{};
    return project.run();
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return run();
}
#endif

#ifdef __unix__
int main()
{
    return run();
}
#endif