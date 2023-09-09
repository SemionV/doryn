#include "dependencies.h"
#include "engine.h"

namespace dory
{
    Engine::Engine(DataContext& context):
        dataContext(context),
        controllers()
    {
    }

    Engine::~Engine()
    {
    }

    void Engine::addController(std::shared_ptr<Controller> controller)
    {
        controllers.push_back(controller);
    }

    bool Engine::update(const TimeSpan& timeStep)
    {
        auto it = controllers.begin();
        auto end = controllers.end();

        for(; it != end; ++it)
        {
            (*it)->update(timeStep, dataContext);
        }

        return dataContext.isStop;
    }

    void Engine::initialize(DataContext& context)
    {
    }
}