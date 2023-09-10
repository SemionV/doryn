#include "dependencies.h"
#include "viewControllerOpenGL.h"

namespace doryOpenGL
{
    ViewControllerOpenGL::ViewControllerOpenGL(std::shared_ptr<dory::View> view):
        ViewController(view)
    {        
    }

    void ViewControllerOpenGL::initialize(dory::DataContext& context)
    {
        ViewController::initialize(context);

        std::cout << "initialize: OpenGL Basic View" << std::endl;        
    }

    void ViewControllerOpenGL::stop(dory::DataContext& context)
    {        
    }

    void ViewControllerOpenGL::update(const dory::TimeSpan& timeStep, dory::DataContext& context)
    {
        auto glfwWindow = std::static_pointer_cast<GlfwWindow>(view->window);

        glfwMakeContextCurrent(glfwWindow->handler);

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(glfwWindow->handler);
    }
}