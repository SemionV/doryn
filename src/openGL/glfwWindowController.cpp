#include "dependencies.h"
#include "glfwWindowController.h"

namespace dory::openGL
{
    GlfwWindowController::GlfwWindowController(std::shared_ptr<EntityRepository<GlfwWindow>> windowRepository,
        std::shared_ptr<GlfwWindowEventHubDispatcher> eventHub):
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

    void GlfwWindowController::update(const TimeSpan& timeStep, DataContext& context)
    {
        glfwPollEvents();

        int count = windowRepository->getEntitiesCount();
        GlfwWindow* entity = windowRepository->getEntities();
        for(int i = 0; i < count; ++i)
        {
            if(glfwWindowShouldClose(entity->handler))
            {
                eventHub->addCase(CloseWindowEventData(entity));
                glfwSetWindowShouldClose(entity->handler, 0);
            }

            entity++;
        }
    }
}