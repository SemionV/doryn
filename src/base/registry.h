#pragma once

#include "domain/dataContext.h"
#include "domain/events/event.h"
#include "base/domain/devices/terminalDevice.h"
#ifdef WIN32
#include "base/domain/devices/standartIoDeviceWin32.h"
#endif
#ifdef __unix__
#include "base/domain/devices/standartIoDeviceUnix.h"
#endif

namespace dory
{
    template<typename TDataContext = domain::DataContext>
    struct EventLayerTypeRegistry
    {
        using EngineDispatcherType = domain::events::engine::Dispatcher<TDataContext>;
        using EngineType = domain::events::engine::Hub<TDataContext>;
        using WindowDispatcherType = domain::events::window::Dispatcher<TDataContext>;
        using WindowType = domain::events::window::Hub<TDataContext>;
        using ApplicationDispatcherType = domain::events::application::Dispatcher<TDataContext>;
        using ApplicationType = domain::events::application::Hub<TDataContext>;
        using StandartInputDispatcherType = domain::events::io::Dispatcher<TDataContext>;
        using StandartInputType = domain::events::io::Hub<TDataContext>;
        using ScriptDispatcherType = domain::events::script::Dispatcher<TDataContext>;
        using ScriptType = domain::events::script::Hub<TDataContext>;
    };

    template<typename T>
    struct EventLayer
    {
        T::EngineDispatcherType engineDispatcher;
        T::EngineType& engine = engineDispatcher;
        T::WindowDispatcherType windowDispatcher;
        T::WindowType& window = windowDispatcher;
        T::ApplicationDispatcherType applicationDispatcher;
        T::ApplicationType& application = applicationDispatcher;
        T::StandartInputDispatcherType standardIoDispatcher;
        T::StandartInputType& standardInput = standardIoDispatcher;
        T::ScriptDispatcherType scriptDispatcher;
        T::ScriptType& script = scriptDispatcher;
    };

    template<typename TDataContext = domain::DataContext>
    struct DeviceLayerTypeRegistry
    {
#ifdef WIN32
        using StandartIODeviceType = domain::devices::ConsoleIODeviceWin32<DataContextType>;
#endif
#ifdef __unix__
        using StandartIODeviceType = domain::devices::ConsoleIODeviceUnix<TDataContext>;
#endif
        using TerminalDeviceType = domain::devices::TerminalDevice<TDataContext, StandartIODeviceType>;
    };

    template<typename T, typename TEventLayer>
    struct DeviceLayer
    {
        T::StandartIODeviceType standardIoDevice;
        T::TerminalDeviceType terminalDevice;

        explicit DeviceLayer(TEventLayer& events):
                standardIoDevice(events.standardIoDispatcher),
                terminalDevice{ standardIoDevice, events.standardInput, events.scriptDispatcher, events.applicationDispatcher }
        {}
    };
}