#include "dependencies.h"
#include "engine.h"

namespace dory
{
    Engine::Engine(DataContext& context, std::shared_ptr<ResourceScopeRepository> resourceScopeRepository):
        dataContext(context),
        controllers(),
        resourceScopeRepository(resourceScopeRepository)
    {
    }

    Engine::~Engine()
    {
    }

    void Engine::addController(Controller* controller)
    {
        controllers.push_back(controller);
    }

    bool Engine::update(const TimeSpan& timeStep)
    {
        resourceScopeRepository->startScope(updateControllersScope);

        std::vector<Controller*>::iterator it = controllers.begin();
        std::vector<Controller*>::iterator end = controllers.end();

        for(; it != end; ++it)
        {
            (*it)->update(timeStep, dataContext);
        }

        resourceScopeRepository->finishScope(updateControllersScope);

        return dataContext.isStop;
    }

    void Engine::initialize(DataContext& context)
    {
        updateControllersScope = resourceScopeRepository->createScope();

        std::vector<Controller*>::iterator it = controllers.begin();
        std::vector<Controller*>::iterator end = controllers.end();

        for(; it != end; ++it)
        {
            (*it)->initialize(context);
        }
    }
}