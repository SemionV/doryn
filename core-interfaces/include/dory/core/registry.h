#pragma once

#include <memory>
#include <map>
#include "services/iFileService.h"
#include "services/iLibraryService.h"
#include "events/eventTypes.h"
#include <dory/core/resources/entity.h>
#include <dory/generic/extension/resourceHandle.h>
#include <dory/core/devices/iStandardIODevice.h>
#include <dory/core/devices/iTerminalDevice.h>
#include <dory/core/repositories/iRepository.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include "services/iLogService.h"
#include "resources/serviceIdentifer.h"
#include <dory/core/services/iSerializationService.h>
#include <dory/generic/extension/registryResourceScope.h>

namespace dory::core
{
    template<typename TServiceInterface, typename TIdentifier = resources::ServiceIdentifier>
    class ResourceHandleController
    {
    private:
        using ServicePtrType = std::shared_ptr<TServiceInterface>;
        std::optional<dory::generic::extension::ResourceHandle<ServicePtrType>> _serviceHandle;
        std::map<TIdentifier, dory::generic::extension::ResourceHandle<ServicePtrType>> _serviceHandles;

    protected:
        void _set(dory::generic::extension::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service)
        {
            _serviceHandle = dory::generic::extension::ResourceHandle(libraryHandle, std::move(service));
        }

        void _set(dory::generic::extension::LibraryHandle libraryHandle, std::shared_ptr<TServiceInterface> service, TIdentifier identifier)
        {
            _serviceHandles.emplace(identifier, dory::generic::extension::ResourceHandle(libraryHandle, std::move(service)));
        }

        void _reset()
        {
            _serviceHandle = {};
        }

        void _reset(TIdentifier identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                _serviceHandles.erase(identifier);
            }
        }

        auto _get()
        {
            if(_serviceHandle)
            {
                return _serviceHandle->lock();
            }

            return dory::generic::extension::ResourceRef<ServicePtrType>{{}, nullptr};
        }

        auto _get(TIdentifier identifier)
        {
            if(_serviceHandles.contains(identifier))
            {
                return _serviceHandles[identifier].lock();
            }

            return dory::generic::extension::ResourceRef<ServicePtrType>{{}, nullptr};
        }
    };

    template<typename TInterface, typename TIdentifier = resources::ServiceIdentifier>
    struct ServiceEntry
    {
        using InterfaceType = TInterface;
        using IdentifierType = TIdentifier;
    };

    template<typename... TServices>
    struct RegistryLayer: public ResourceHandleController<typename TServices::InterfaceType, typename TServices::IdentifierType>...
    {
    public:
        template<typename TInterface>
        void set(dory::generic::extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->ResourceHandleController<TInterface, resources::ServiceIdentifier>::_set(libraryHandle, service);
        }

        template<typename TInterface, auto identifier>
        void set(dory::generic::extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service)
        {
            this->ResourceHandleController<TInterface, decltype(identifier)>::_set(libraryHandle, service, identifier);
        }

        template<typename TInterface, typename TIdentifier>
        void set(dory::generic::extension::LibraryHandle libraryHandle, std::shared_ptr<TInterface> service, TIdentifier identifier)
        {
            this->ResourceHandleController<TInterface, TIdentifier>::_set(libraryHandle, service, identifier);
        }

        template<typename TService>
        void reset()
        {
            this->ResourceHandleController<TService, resources::ServiceIdentifier>::_reset();
        }

        template<typename TService, auto identifier>
        void reset()
        {
            this->ResourceHandleController<TService, decltype(identifier)>::_reset(identifier);
        }

        template<typename TService, typename TIdentifier>
        void reset(TIdentifier identifier)
        {
            this->ResourceHandleController<TService, TIdentifier>::_reset(identifier);
        }

        template<typename TService>
        auto get()
        {
            return this->ResourceHandleController<TService, resources::ServiceIdentifier>::_get();;
        }

        template<typename TService, auto identifier>
        auto get()
        {
            return this->ResourceHandleController<TService, decltype(identifier)>::_get(identifier);;
        }

        template<typename TService, typename TIdentifier>
        auto get(TIdentifier identifier)
        {
            return this->ResourceHandleController<TService, TIdentifier>::_get(identifier);;
        }
    };

    namespace repositories
    {
        using ICameraRepository = repositories::IRepository<resources::entity::Camera>;
        using IViewRepository = repositories::IRepository<resources::entity::View>;
        using IWindowRepository = repositories::IRepository<resources::entity::Window>;
        using IPipelineNodeRepository = core::repositories::IRepository<core::resources::entity::PipelineNode<core::resources::DataContext>>;
    }

    enum class Logger
    {
        App,
        Config
    };

    enum class DataFormat
    {
        Yaml,
        Json
    };

    struct Registry: public RegistryLayer<
            /*Events*/
            ServiceEntry<events::mainController::Bundle::IDispatcher>,
            ServiceEntry<events::mainController::Bundle::IListener>,
            ServiceEntry<events::application::Bundle::IDispatcher>,
            ServiceEntry<events::application::Bundle::IListener>,
            ServiceEntry<events::io::Bundle::IDispatcher>,
            ServiceEntry<events::io::Bundle::IListener>,
            ServiceEntry<events::script::Bundle::IDispatcher>,
            ServiceEntry<events::script::Bundle::IListener>,
            ServiceEntry<events::window::Bundle::IDispatcher>,
            ServiceEntry<events::window::Bundle::IListener>,
            /*Devices*/
            ServiceEntry<devices::IStandardIODevice>,
            ServiceEntry<devices::ITerminalDevice>,
            /*Repositories*/
            ServiceEntry<repositories::ICameraRepository>,
            ServiceEntry<repositories::IViewRepository>,
            ServiceEntry<repositories::IWindowRepository>,
            ServiceEntry<repositories::IPipelineRepository>,
            ServiceEntry<repositories::IPipelineNodeRepository>,
            /*Services*/
            ServiceEntry<services::ILibraryService>,
            ServiceEntry<services::IFileService>,
            ServiceEntry<services::IMultiSinkLogService, Logger>,
            ServiceEntry<services::serialization::ISerializer, DataFormat>>
    {};

    template<typename T>
    using RegistryResourceScope = dory::generic::extension::RegistryResourceScope<dory::generic::extension::RegistryResourceScopePolicy<T, core::Registry, core::resources::ServiceIdentifier>>;
}