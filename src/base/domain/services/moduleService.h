#pragma once

#include <utility>

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::services::module
{
    template<typename TImplementation>
    class IModuleLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TModuleContext>
        std::unique_ptr<ILoadableModule<TModuleContext>> load(const ModuleHandle& moduleHandle)
        {
            return this->toImplementation()->template loadImpl<TModuleContext>(moduleHandle);
        }
    };

    template<typename TLogger>
    class ModuleLoader: public IModuleLoader<ModuleLoader<TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        std::map<std::string, boost::dll::shared_library> libraries;

    public:
        explicit ModuleLoader(LoggerType& logger):
            logger(logger)
        {}

        template<typename TModuleContext>
        std::unique_ptr<ILoadableModule<TModuleContext>> loadImpl(const ModuleHandle& moduleHandle)
        {
            const auto& moduleName = moduleHandle.name;
            const auto& modulePath = moduleHandle.path;

            logger.information(fmt::format(R"(Load module "{0}" from "{1}")", moduleHandle.name, modulePath.string()));

            constexpr auto errorPattern = R"(Error on loading an instance of module "{0}" from "{1}": {2})";
            try
            {
                auto insertion = libraries.emplace(moduleName, boost::dll::shared_library{});
                if(insertion.second)
                {
                    auto& library = insertion.first->second;
                    auto path = modulePath.string() + systemSharedLibraryFileExtension;
                    library.load(path);

                    auto moduleFactory = library.template get<LoadableModuleFactory<TModuleContext>>(loadableModuleFactoryFunctionName);
                    return moduleFactory();
                }
                else
                {
                    logger.warning(fmt::format("Module is loaded already: {0}, {1}", moduleName, modulePath.string()));
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

            return nullptr;
        }
    };

    template<typename TImplementation>
    class IModulesService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TModuleContext>
        void loadModules(const std::map<std::string, std::string>& modules, TModuleContext& modulesContext)
        {
            this->toImplementation()->loadModulesImpl(modules, modulesContext);
        }

        void reloadModule(const std::string& moduleName)
        {
            this->toImplementation()->reloadModuleImpl(moduleName);
        }

        void unloadModule(const std::string& moduleName)
        {
            this->toImplementation()->unloaddModuleImpl(moduleName);
        }
    };

    template<typename TModuleLoader, typename TLogger>
    class ModulesService: public IModulesService<ModulesService<TModuleLoader, TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        using ModelLoaderType = IModuleLoader<TModuleLoader>;
        ModelLoaderType& moduleLoader;

        std::map<std::string, ModuleHandle> moduleHandles;
        std::map<std::string, IModule> loadedModules;

        void unload(const std::string& moduleName)
        {
            if(moduleHandles.contains(moduleName))
            {
                auto& handle = moduleHandles[moduleName];
                if(handle.isMultithreaded)
                {
                    auto lock = std::lock_guard<std::mutex>{handle.mutex};
                    moduleHandles.erase(moduleName);
                    handle.isLoaded = false;
                }
                else
                {
                    moduleHandles.erase(moduleName);
                    handle.isLoaded = false;
                }
            }
        }

        template<typename TModuleContext>
        void load(const std::string& moduleName, const std::filesystem::path& modulePath, TModuleContext& moduleContext)
        {
            auto insertion = moduleHandles.emplace(moduleName, ModuleHandle{ moduleName, modulePath });
            if(insertion.second)
            {
                auto& handle = insertion.first->second;
                auto module = moduleLoader.template load<TModuleContext>(modulePath, moduleName, handle);
                if(module)
                {
                    constexpr auto errorPattern = R"(Error on running module "{0}" from "{1}": {2})";
                    try
                    {
                        module->load(handle, moduleContext);
                        loadedModules[moduleName] = module;
                    }
                    catch(const std::exception&e)
                    {
                        logger.error(fmt::format(errorPattern, moduleName, modulePath.string(), e.what()));
                    }
                    catch(...)
                    {
                        logger.error(fmt::format(errorPattern, moduleName, modulePath.string(), "unknown exception type"));
                    }
                }
                else
                {
                    logger.error(fmt::format(R"(Module is not loaded: "{0}" from "{1}")", moduleName, modulePath.string()));
                }
            }
            else
            {
                logger.warning(fmt::format("Module handle is already in place: {0}, {1}", moduleName, modulePath.string()));
            }
        }

        template<typename TModuleContext>
        void reload(const std::string& moduleName, const std::string& modulePath, TModuleContext& moduleContext)
        {
            unload(moduleName);
            load(moduleName, modulePath, moduleContext);
        }

    public:
        ModulesService(ModelLoaderType& moduleLoader, LoggerType& logger):
                moduleLoader(moduleLoader),
                logger(logger)
        {}

        template<typename TModuleContext>
        void loadModulesImpl(const std::map<std::string, std::string>& modules, TModuleContext& moduleContext)
        {
            for(auto& pair : modules)
            {
                reload(pair.first, pair.second, moduleContext);
            }
        }

        void unloadModuleImpl(const std::string& moduleName)
        {
            unload(moduleName);
        }
    };
}