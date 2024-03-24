#include "dependencies.h"
#include "base/base.h"
#include "openGL/openGL.h"
#include "serviceLocator.h"
#include "project.h"
#include "projectDataContext.h"

#include "registry.h"
#include "project2.h"

namespace testApp
{
    int runProject()
    {
        ProjectDataContext context;
        ServiceLocator services;

        Project project{services};
        project.run(context);

        std::cout << "Session is over." << std::endl;

        return 0;
    }

    int runProject2()
    {
        using Services = ServiceDependencies<ProjectDataContext>;

        auto services = Services::ServiceContainerType{};
        auto context = ProjectDataContext{};

        services.get<Services::ProjectDep>().run(context);

        return 0;
    }
}

//linux entry point
/*int main()
{
    return runProject();
}*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR szArgs, int nCmdShow)
{
    return testApp::runProject2();
    //return testApp::runProject();
}