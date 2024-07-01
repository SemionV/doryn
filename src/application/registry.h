#pragma once

#include <dory/base.h>
#include <dory/opengl.h>

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

        using StandartIODeviceType = domain::devices::ConsoleIODevice<TDataContext>;
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

    template<typename TDataContext = domain::DataContext>
    struct RepositoryLayerTypeRegistry
    {
        using CameraRepositoryType = domain::EntityRepository<domain::entity::Camera>;
        using ViewRepositoryType = domain::EntityRepository<domain::entity::View>;
        using WindowRepositoryType = domain::EntityRepository<openGL::GlfwWindow>;
        using PipelineRepositoryType = domain::repositories::PipelineRepository<TDataContext, domain::entity::PipelineNode<TDataContext>>;
    };

    template<typename T>
    struct RepositoryLayer
    {
        T::CameraRepositoryType cameras;
        T::ViewRepositoryType views;
        T::WindowRepositoryType windows;
        T::PipelineRepositoryType pipelines;
    };
}