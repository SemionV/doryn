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

    void Engine::addController(Controller* controller)
    {
        controllers.push_back(controller);
    }

    bool Engine::update(const TimeSpan& timeStep)
    {
        std::vector<Controller*>::iterator it = controllers.begin();
        std::vector<Controller*>::iterator end = controllers.end();

        for(; it != end; ++it)
        {
            (*it)->update(timeStep, dataContext);
        }

        return dataContext.isStop;
    }

    void Engine::initialize(DataContext& context)
    {
        std::vector<Controller*>::iterator it = controllers.begin();
        std::vector<Controller*>::iterator end = controllers.end();

        for(; it != end; ++it)
        {
            (*it)->initialize(context);
        }
    }
}