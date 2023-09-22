#include "dependencies.h"
#include "glfwWindowController.h"

namespace dory::openGL
{
    GlfwWindowController::GlfwWindowController(std::shared_ptr<RepositoryReader<GlfwWindow>> windowRepository,
        std::shared_ptr<events::WindowEventHubDispatcher> eventHub):
            windowRepository(windowRepository),
            eventHub(eventHub)
    {}

    bool GlfwWindowController::initialize(domain::entity::IdType referenceId, DataContext& context)
    {
        return glfwInit();
    }
    
    void GlfwWindowController::stop(domain::entity::IdType referenceId, DataContext& context)
    {
        glfwTerminate();
    }

    void GlfwWindowController::update(domain::entity::IdType referenceId, const TimeSpan& timeStep, DataContext& context)
    {
        glfwPollEvents();

        auto iterator = windowRepository->getTraverseIterator();
        auto window = iterator->next();
        while(window)
        {
            if(glfwWindowShouldClose(window->handler))
            {
                eventHub->addCase(events::CloseWindowEventData(window->id));
                glfwSetWindowShouldClose(window->handler, 0);
            }

            window = iterator->next();
        }

        eventHub->submit(context);
    }
}