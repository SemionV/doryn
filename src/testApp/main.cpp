#include "dependencies.h"
#include "project.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    auto project = testApp::Project{};
    return project.run();
}