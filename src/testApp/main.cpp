#include "dependencies.h"
#include "base/base.h"
#include "openGL/openGL.h"
#include "win32/win32.h"
#include "serviceLocator.h"
#include "project.h"
#include "projectDataContext.h"

namespace testApp
{
    int runProject()
    {
        ProjectDataContext context;

        auto serviceLocator = std::make_shared<testApp::ServiceLocator<ProjectDataContext>>();
        serviceLocator->configure();

        testApp::Project<ProjectDataContext> project(serviceLocator);
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