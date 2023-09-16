#include "base/dependencies.h"
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
        controllers.emplace_back(controller);
    }

    void Engine::removeController(std::shared_ptr<Controller> controller)
    {
        auto it = controllers.begin();
        auto end = controllers.end();

        for(; it != end; ++it)
        {
            if((*it) == controller)
            {
                controllers.erase(it);
                break;
            }
        }
    }

    bool Engine::update(const TimeSpan& timeStep)
    {
        auto size = controllers.size();
        for(std::size_t i = 0; i < size; ++i)
        {
            //TODO: pass proper controller reference id
            controllers[i]->update(0, timeStep, dataContext);
        }

        return dataContext.isStop;
    }

    void Engine::initialize(DataContext& context)
    {
    }
}