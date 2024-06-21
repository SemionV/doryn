#pragma once

#include <utility>

#include "dependencies.h"
#include "typeComponents.h"
#include "memory.h"

namespace dory
{
    class

    class ILibrary: NonCopyable
    {
    public:
#ifdef WIN32
        const constexpr static std::string_view& systemSharedLibraryFileExtension = ".dll";
#endif

#ifdef __unix__
        const constexpr static std::string_view& systemSharedLibraryFileExtension = ".so";
#endif

    public:
        const std::string name;
        const std::filesystem::path path;

        virtual bool isLoaded() = 0;
    };

    class IModule
    {
    public:
        virtual ~IModule() = default;
    };

    template<typename TModuleContext>
    class ILoadableModule: public IModule
    {
    public:
        ~ILoadableModule() override = default;
        virtual void attach(std::weak_ptr<ILibrary> library, /*CustomAllocator<>,*/ TModuleContext& moduleContext) = 0;
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

    template<typename TResource>
    class ResourceHandle: NonCopyable
    {
    private:
        std::shared_ptr<ILibrary> _library;
        TResource* _resource;

    public:
        explicit ResourceHandle(std::shared_ptr<ILibrary> library, TResource* resource):
            _library(std::move(library)),
            _resource(resource)
        {}

        explicit operator bool()
        {
            return _library.operator bool() && _resource;
        }

        TResource* operator->()
        {
            assert((bool)this);
            return _resource;
        }

        TResource& operator*()
        {
            assert((bool)this);
            return *_resource;
        }
    };

    template<typename TResource>
    class LibraryResource: NonCopyable
    {
    private:
        std::unique_ptr<TResource> _resource;
        std::weak_ptr<ILibrary> _library;

    public:
        explicit LibraryResource(std::weak_ptr<ILibrary> library, std::unique_ptr<TResource> resource):
            _library(std::move(library)),
            _resource(std::move(resource))
        {}

        ResourceHandle<TResource> lock()
        {
            auto library = _library.lock();
            return ResourceHandle<TResource>{ library && library->isLoaded() ? library : nullptr, _resource.get() };
        }
    };
}
