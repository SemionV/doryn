#pragma once

#include <dory/engine/dependencies.h>
#include <dory/engine/typeComponents.h>

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
        virtual void attach(LibraryHandle library) = 0;
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

        ILibrary(std::string  libraryName, std::filesystem::path libraryPath):
            name(std::move(libraryName)), path(std::move(libraryPath))
        {}

        virtual bool isLoaded() = 0;
    };

    template<typename TImplementation>
    class Library: public StaticInterface<TImplementation>, public ILibrary
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

    public:
        explicit Library(const std::string& libraryName, const std::filesystem::path& libraryPath):
                ILibrary(libraryName, libraryPath)
        {}

        inline bool isLoaded() noexcept final
        {
            return this->toImplementation()->isLoadedImpl();
        }

        void unloadModules()
        {
            _modules.clear();
        }

        template<typename TModuleContext>
        std::shared_ptr<IDynamicModule<TModuleContext>> loadModule(const std::string moduleName, TModuleContext& moduleContext)
        {
            if(_modules.contains(moduleName))
            {
                _modules.erase(moduleName);
            }

            auto module = this->toImplementation()->template loadModuleImpl<TModuleContext>(moduleName, moduleContext);
            if(module)
            {
                _modules[moduleName] = module;
            }

            return module;
        }
    };

    class DynamicLibrary: public Library<DynamicLibrary>
    {
    private:
        constexpr const static std::string_view dynamicModuleFactoryFunctionName = "dynamicModuleFactory";
        template<typename TModuleContext>
        using LoadableModuleFactory = std::shared_ptr<IDynamicModule<TModuleContext>>(const std::string& moduleName, TModuleContext& moduleContext);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;

    public:
        explicit DynamicLibrary(const std::string& libraryName, const std::filesystem::path& libraryPath):
                Library(libraryName, libraryPath)
        {}

        ~DynamicLibrary()
        {
            unloadModules();
            _dll.unload();
        }

        bool isLoadedImpl() noexcept
        {
            return _isLoaded;
        }

        template<typename TModuleContext>
        std::shared_ptr<IDynamicModule<TModuleContext>> loadModuleImpl(const std::string moduleName, TModuleContext& moduleContext)
        {
            assert(_dll.is_loaded());

            auto moduleFactory = _dll.template get<LoadableModuleFactory<TModuleContext>>(std::string{ dynamicModuleFactoryFunctionName });
            return moduleFactory(moduleName, moduleContext);
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
            _dll.load(path, boost::dll::load_mode::rtld_local);
            _isLoaded = true;
        }
    };

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
        TResource _resource;

    public:
        explicit ResourceRef(std::optional<std::shared_ptr<ILibrary>> library, TResource resource):
            _library(std::move(library)),
            _resource(std::move(resource))
        {}

        explicit operator bool()
        {
            return (!_library || (bool) *_library);
        }

        inline TResource& operator*()
        {
            assert((bool)this);
            return _resource;
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
        TResource _resource;
        LibraryHandle _library;

    public:
        explicit ResourceHandle(LibraryHandle library, TResource resource):
            _library(std::move(library)),
            _resource(resource)
        {}

        inline ResourceRef<TResource> lock() final
        {
            auto library = _library.lock();
            return ResourceRef<TResource>{ library && library->isLoaded() ? library : nullptr, _resource };
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
            return ResourceRef<TResource>{{}, _resource };
        }
    };

    template<typename TResource>
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(LibraryHandle& libraryHandle, TResource resource)
    {
        return std::make_shared<dory::ResourceHandle<TResource>>(libraryHandle, resource);
    }

    template<typename TResource, typename... Ts>
    std::shared_ptr<IResourceHandle<TResource>> makeResourceHandle(Ts&&... resourceParameters)
    {
        return std::make_shared<StaticResourceHandle<TResource>>(std::forward<Ts>(resourceParameters)...);
    }
}
