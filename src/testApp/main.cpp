#include "dependencies.h"
#include "project.h"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    auto project = testApp::Project{};
    return project.run();
}
#endif

#ifdef __unix__
int main()
{
    auto project = testApp::Project{};
    return project.run();
}
#endif