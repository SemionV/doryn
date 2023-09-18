#include "dependencies.h"
#include "glfwWindowController.h"

namespace dory::openGL
{
    GlfwWindowController::GlfwWindowController(std::shared_ptr<RepositoryReader<GlfwWindow>> windowRepository,
        std::shared_ptr<WindowEventHubDispatcher> eventHub):
            windowRepository(windowRepository),
            eventHub(eventHub)
    {}

    bool GlfwWindowController::initialize(DataContext& context)
    {
        return glfwInit();
    }
    
    void GlfwWindowController::stop(DataContext& context)
    {
        glfwTerminate();
    }

    void GlfwWindowController::update(const int referenceId, const TimeSpan& timeStep, DataContext& context)
    {
        glfwPollEvents();

        auto iterator = windowRepository->getTraverseIterator();
        auto entity = iterator->next();
        while(entity.has_value())
        {
            GlfwWindow& window = entity.value();

            if(glfwWindowShouldClose(window.handler))
            {
                eventHub->addCase(window.id);
                glfwSetWindowShouldClose(window.handler, 0);
            }

            entity = iterator->next();
        }

        eventHub->submit(context);
    }
}