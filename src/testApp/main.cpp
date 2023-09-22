#include "dependencies.h"
#include "project.h"


#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

namespace testApp
{
    int runProject()
    {
        Project project;

        project.configure();
        project.run();

        std::cout << "Session is over." << std::endl;

        return 0;
    }
}

/*int main()
{
    return runDory();
}*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return testApp::runProject();
}