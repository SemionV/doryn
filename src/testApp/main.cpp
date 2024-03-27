#include "dependencies.h"
#include "project.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return testApp::Project{}.run();
}