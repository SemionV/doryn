#include "dependencies.h"
#include "serviceLocator.h"
#include "project.h"
#include "projectDataContext.h"


#include <windows.h>

#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

namespace testApp
{
    int runProject()
    {
        ProjectDataContext context;

        testApp::Project<ProjectDataContext> project;
        project.run(context);

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