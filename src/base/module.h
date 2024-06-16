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

    struct ModuleHandle
    {
        const std::string name;
        std::filesystem::path path;
        std::mutex mutex;
        //TODO: check if the module is unloaded on destruction, terminate if not
        bool isLoaded = false;
        bool isMultithreaded = false;

        explicit ModuleHandle(std::string  name, std::filesystem::path  path):
                name(std::move(name)),
                path(std::move(path))
        {}
    };

    class IModule
    {
    public:
        virtual ~IModule() = default;
    };

    template<typename TModuleContext>
    class ILoadableModule
    {
    public:
        virtual ~ILoadableModule() = default;
        virtual void load(const ModuleHandle& moduleState, TModuleContext& moduleContext) = 0;
    };

    template<typename TModuleContext>
    using LoadableModuleFactory = std::unique_ptr<ILoadableModule<TModuleContext>>();

    const static std::string loadableModuleFactoryFunctionName = "loadableModuleFactory";
}
