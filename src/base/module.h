#pragma once

#include <utility>

#include "dependencies.h"
#include "typeComponents.h"
#include "memory.h"

namespace dory
{
    class IModule
    {
    public:
        virtual ~IModule() = default;
    };

    class LibraryHandle;

    template<typename TModuleContext>
    class IDynamicModule: public IModule
    {
    public:
        ~IDynamicModule() override = default;
        virtual void attach(LibraryHandle library, TModuleContext& moduleContext) = 0;
    };

    template<typename TModuleContext>
    class IExecutableModule: public IModule
    {
    public:
        ~IExecutableModule() override = default;
        virtual int run(TModuleContext& moduleContext) = 0;
    };

    class ILibrary: NonCopyable
    {
    public:
#ifdef WIN32
        const constexpr static std::string_view systemSharedLibraryFileExtension = ".dll";
#endif

#ifdef __unix__
        const constexpr static std::string_view systemSharedLibraryFileExtension = ".so";
#endif

    public:
        const std::string name;
        const std::filesystem::path path;

        ILibrary() = default;

        ILibrary(const std::string& libraryName, const std::filesystem::path& libraryPath):
            name(libraryName), path(libraryPath)
        {}

        virtual bool isLoaded() = 0;
    };

    class DynamicLibrary: public ILibrary
    {
    private:
        constexpr const static std::string_view dynamicModuleFactoryFunctionName = "dynamicModuleFactory";
        template<typename TModuleContext>
        using LoadableModuleFactory = std::shared_ptr<IDynamicModule<TModuleContext>>(const std::string& moduleName);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

    public:
        explicit DynamicLibrary(const std::string& libraryName, const std::filesystem::path& libraryPath):
                ILibrary(libraryName, libraryPath)
        {}

        inline bool isLoaded() noexcept final
        {
            return _isLoaded;
        }

        void unload()
        {
            assert(_isLoaded);
            _isLoaded = false;
        }

        void load(const std::filesystem::path& libraryPath)
        {
            assert(!_isLoaded);

            auto path = libraryPath.string() + std::string { ILibrary::systemSharedLibraryFileExtension };
            _dll.load(path);
            _isLoaded = true;
        }

        template<typename TModuleContext>
        std::shared_ptr<IDynamicModule<TModuleContext>> loadModule(const std::string moduleName)
        {
            assert(_dll.is_loaded());

            if(_modules.contains(moduleName))
            {
                _modules.erase(moduleName);
            }

            auto moduleFactory = _dll.template get<LoadableModuleFactory<TModuleContext>>(std::string{ dynamicModuleFactoryFunctionName });
            auto module = moduleFactory(moduleName);
            if(module)
            {
                _modules[moduleName] = module;
            }

            return module;
        }
    };

    template<typename TModuleContext>
    class ILoadableModule: public IModule
    {
    public:
        ~ILoadableModule() override = default;
        virtual void attach(std::weak_ptr<ILibrary> library, TModuleContext& moduleContext) = 0;
        virtual void detach(TModuleContext& moduleContext) = 0;
    };

    template<typename TModuleContext>
    class DynamicLinkLibrary: public ILibrary
    {
    public:
        virtual ~DynamicLinkLibrary() = default;

        virtual std::shared_ptr<ILoadableModule<TModuleContext>> load(const std::filesystem::path& libraryPath) = 0;
        virtual std::shared_ptr<ILoadableModule<TModuleContext>> unload() = 0;
    };

    template<typename TModuleContext>
    using LoadableModuleFactory = std::shared_ptr<ILoadableModule<TModuleContext>>();

    const static std::string loadableModuleFactoryFunctionName = "loadableModuleFactory";

    template<typename P, typename... Args>
    bool invokeModuleProcedure(std::optional<std::weak_ptr<ILibrary>>& libraryOption, P procedure, Args... arguments)
    {
        if(libraryOption)
        {
            std::shared_ptr<ILibrary> library = (*libraryOption).lock();
            if(library && library->isLoaded())
            {
                std::invoke(procedure, arguments...);
            }
            else
            {
                return false;
            }
        }
        else
        {
            std::invoke(procedure, arguments...);
        }

        return true;
    }

    class LibraryHandle
    {
    private:
        std::optional<std::weak_ptr<ILibrary>> _libraryOption;

        std::shared_ptr<ILibrary> lock()
        {
            assert(!isStatic());
            return (*_libraryOption).lock();
        }

    public:
        explicit LibraryHandle() = default;

        explicit LibraryHandle(std::weak_ptr<ILibrary> library):
                _libraryOption(std::move(library))
        {}

        bool isStatic()
        {
            return !(bool)_libraryOption;
        }

        template<typename U>
        friend class ResourceHandle;
    };

    template<typename TResource>
    class ResourceRef
    {
    private:
        std::optional<std::shared_ptr<ILibrary>> _library;
        TResource* _resource;

    public:
        explicit ResourceRef(std::optional<std::shared_ptr<ILibrary>> library, TResource* resource):
            _library(std::move(library)),
            _resource(resource)
        {}

        explicit operator bool()
        {
            return (!_library || (bool) *_library) && _resource;
        }

        inline TResource* operator->()
        {
            assert((bool)this);
            return _resource;
        }

        inline TResource& operator*()
        {
            assert((bool)this);
            return *_resource;
        }
    };

    template<typename TResource>
    class IResourceHandle: public NonCopyable
    {
    public:
        virtual ~IResourceHandle() = default;

        virtual ResourceRef<TResource> lock() = 0;
    };

    template<typename TResource>
    class ResourceHandle: public IResourceHandle<TResource>
    {
    private:
        TResource* _resource;
        LibraryHandle _library;

    public:
        explicit ResourceHandle(LibraryHandle library, TResource* resource):
            _library(std::move(library)),
            _resource(resource)
        {}

        inline ResourceRef<TResource> lock() final
        {
            auto library = _library.lock();
            return ResourceRef<TResource>{library && library->isLoaded() ? library : nullptr, _resource };
        }
    };

    template<typename TResource>
    class StaticResourceHandle: public IResourceHandle<TResource>
    {
    private:
        TResource _resource;

    public:
        template<typename... Ts>
        explicit StaticResourceHandle(Ts&&... args):
                _resource{std::forward<Ts>(args)...}
        {}

        inline ResourceRef<TResource> lock() final
        {
            return ResourceRef<TResource>{{}, &_resource };
        }
    };

    template<typename TResource>
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(LibraryHandle& libraryHandle, TResource* resource)
    {
        return std::make_shared<dory::ResourceHandle<TResource>>(libraryHandle, resource);
    }

    template<typename TResource, typename... Ts>
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(Ts&&... resourceParameters)
    {
        return std::make_shared<StaticResourceHandle<TResource>>(std::forward<Ts>(resourceParameters)...);
    }
}
