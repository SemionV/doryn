#pragma once

#include "dataContext.h"
#include <dory/engine/events/eventTypes.h>
#include <dory/engine/events/mainController.h>

namespace dory::engine::events
{
    struct EventTypeRegistry
    {
        using EngineDispatcherType = mainController::Dispatcher<DataContextType>;
        using MainControllerType = mainController::Hub<DataContextType>;
        using WindowDispatcherType = window::Dispatcher<DataContextType>;
        using WindowType = window::Hub<DataContextType>;
        using ApplicationDispatcherType = application::Dispatcher<DataContextType>;
        using ApplicationType = application::Hub<DataContextType>;
        using StandartInputDispatcherType = io::Dispatcher<DataContextType>;
        using StandartInputType = io::Hub<DataContextType>;
        using ScriptDispatcherType = script::Dispatcher<DataContextType>;
        using ScriptType = script::Hub<DataContextType>;
    };

    struct EventLayer
    {
        EventTypeRegistry::EngineDispatcherType mainControllerDispatcher;
        EventTypeRegistry::MainControllerType& mainController = mainControllerDispatcher;
        EventTypeRegistry::WindowDispatcherType windowDispatcher;
        EventTypeRegistry::WindowType& window = windowDispatcher;
        EventTypeRegistry::ApplicationDispatcherType applicationDispatcher;
        EventTypeRegistry::ApplicationType& application = applicationDispatcher;
        EventTypeRegistry::StandartInputDispatcherType standardIoDispatcher;
        EventTypeRegistry::StandartInputType& standardInput = standardIoDispatcher;
        EventTypeRegistry::ScriptDispatcherType scriptDispatcher;
        EventTypeRegistry::ScriptType& script = scriptDispatcher;
    };
}