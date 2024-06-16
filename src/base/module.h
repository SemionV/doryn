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
        bool isLoaded = false;
        bool hotReloadEnabled = false;

        explicit ModuleHandle(std::string  name, std::filesystem::path  path):
                name(std::move(name)),
                path(std::move(path))
        {}
    };

    template<typename TRegistry>
    class IModule
    {
    public:
        virtual ~IModule() = default;

        virtual void run(const ModuleHandle& moduleState, TRegistry& registry) = 0;
    };

    template<typename TModuleContext>
    using ModuleFactory = std::unique_ptr<IModule<TModuleContext>>();

    const static std::string moduleFactoryFunctionName = "moduleFactory";
}
