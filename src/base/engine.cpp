#include "engine.h"

namespace dory
{
    Engine::Engine():
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
}