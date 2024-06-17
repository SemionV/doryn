#pragma once

#include <utility>

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::services::module
{
    struct BoostSharedLibrary: public SharedLibrary
    {
        boost::dll::shared_library dll;
    };

    template<typename TImplementation>
    class IModuleService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TModuleContext>
        void load(const std::string& moduleName, const std::filesystem::path& modulePath, TModuleContext& moduleContext)
        {
            this->toImplementation()->template loadImpl(moduleName, modulePath, moduleContext);
        }

        void unload(const std::string& moduleName)
        {
            this->toImplementation()->template unloadImpl(moduleName);
        }
    };

    template<typename TLogger>
    class ModuleService: public IModuleService<ModuleService<TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        //Declaration order is very important for proper destruction and unloading of shared libraries
        std::map<std::string, std::shared_ptr<ModuleHandle>> moduleHandles;
        std::map<std::string, std::unique_ptr<IModule>> modules;

    public:
        explicit ModuleService(LoggerType& logger):
            logger(logger)
        {}

        template<typename TModuleContext>
        void loadImpl(const std::string& moduleName, const std::filesystem::path& modulePath, TModuleContext& moduleContext)
        {
            unloadImpl(moduleName);

            logger.information(fmt::format(R"(Load module "{0}" from "{1}")", moduleName, modulePath.string()));

            constexpr auto errorPattern = R"(Error on loading module "{0}" from "{1}": {2})";
            try
            {
                auto library = std::make_unique<BoostSharedLibrary>();

                auto path = modulePath.string() + systemSharedLibraryFileExtension;
                library->dll.load(path);

                auto moduleFactory = library->dll.template get<LoadableModuleFactory<TModuleContext>>(loadableModuleFactoryFunctionName);
                auto moduleHandle = std::make_shared<ModuleHandle>(moduleName, modulePath, std::move(library));

                auto module = moduleFactory();
                if(module)
                {
                    module->load(moduleHandle, moduleContext);

                    modules[moduleName] = std::move(module);
                    moduleHandles[moduleName] = moduleHandle;
                }
            }
            catch(const std::exception& e)
            {
                logger.error(fmt::format(errorPattern, moduleName, modulePath.string(), e.what()));
            }
            catch(...)
            {
                logger.error(fmt::format(errorPattern, moduleName, modulePath.string(), "unknown exception type"));
            }
        }

        void unloadImpl(const std::string& moduleName)
        {
            if(modules.contains(moduleName))
            {
                logger.information(fmt::format(R"(Unload module "{0}"")", moduleName));
                modules.erase(moduleName);
            }

            if(moduleHandles.contains(moduleName))
            {
                auto moduleHandle = moduleHandles[moduleName];
                moduleHandle->isUnloading = true;

                logger.information(fmt::format(R"(Unload library "{0}"")", moduleName));
                moduleHandles.erase(moduleName);
            }
        }
    };
}