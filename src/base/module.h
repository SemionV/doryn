#pragma once

#include "dependencies.h"

namespace dory
{

#ifdef WIN32
    const static std::string& systemSharedLibraryFileExtension = ".dll";
#endif

#ifdef __unix__
    const static std::string& systemSharedLibraryFileExtension = ".so";
#endif

    struct SharedLibrary
    {
        virtual ~SharedLibrary() = default;
    };

    struct ModuleHandle
    {
        const std::string name;
        const std::filesystem::path path;
        const std::unique_ptr<SharedLibrary> library;
        std::atomic<bool> isUnloading = false;

        explicit ModuleHandle(std::string  name,
                              std::filesystem::path  path,
                              std::unique_ptr<SharedLibrary> library):
                name(std::move(name)),
                path(std::move(path)),
                library(std::move(library))
        {}
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
        virtual void load(std::shared_ptr<ModuleHandle> moduleHandle, TModuleContext& moduleContext) = 0;
    };

    template<typename TModuleContext>
    using LoadableModuleFactory = std::unique_ptr<ILoadableModule<TModuleContext>>();

    const static std::string loadableModuleFactoryFunctionName = "loadableModuleFactory";

    template<typename P, typename... Args>
    bool invokeModuleProcedure(std::optional<std::weak_ptr<ModuleHandle>>& moduleHandleOption, P procedure, Args... arguments)
    {
        if(moduleHandleOption)
        {
            std::shared_ptr<ModuleHandle> moduleHandle = (*moduleHandleOption).lock();
            if(moduleHandle && !moduleHandle->isUnloading)
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
}
