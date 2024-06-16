#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::services::module
{
    ModuleStateType makeHandleState()
    {
        return std::make_shared<ModuleStateBasicType>();
    }

    template<typename TModule>
    struct ModuleHandle
    {
        const std::string name;
        std::filesystem::path path;

        /*here the declaration order of the members below is very important for
        step by step proper dereferencing of a module and unload of the module from memory*/
        boost::dll::shared_library library;
        std::unique_ptr<TModule> module;
        //TODO: use bool flag instead of shared_ptr
        ModuleStateType state;
        //TODO: use more global mutex in order to avoid too many locks while firing many events or running amny controller
        // symply lock a global mutex(perphaps resided in ModulesService) and block unloading of modules while running a bundle of actions
        std::mutex mutex;
        bool hotReloadEnabled = false;
    };

    template<typename TImplementation>
    class IModuleLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TModule>
        void load(ModuleHandle<TModule>& moduleHandle)
        {
            return this->toImplementation()->template loadImpl<TModule>(moduleHandle);
        }
    };

    template<typename TLogger>
    class ModuleLoader: public IModuleLoader<ModuleLoader<TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

    public:
        explicit ModuleLoader(LoggerType& logger):
            logger(logger)
        {}

        template<typename TModule>
        void loadImpl(ModuleHandle<TModule>& moduleHandle)
        {
            const auto& moduleName = moduleHandle.name;
            const auto& modulePath = moduleHandle.path;

            logger.information(fmt::format(R"(Load module "{0}" from "{1}")", moduleHandle.name, modulePath.string()));

            constexpr auto errorPattern = R"(Error on loading an instance of module "{0}" from "{1}": {2})";
            try
            {
                auto& library = moduleHandle.library;
                library.load((std::filesystem::path{modulePath} /= systemSharedLibraryFileExtension).string());

                auto moduleFactory = library.template get<ModuleFactory<TModule>>(moduleFactoryFunctionName);
                moduleHandle.module = moduleFactory();
                moduleHandle.state = makeHandleState();
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
    };

    template<typename TImplementation>
    class IModulesService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TServiceRegistry>
        void loadModules(const std::map<std::string, std::string>& modules, TServiceRegistry& serviceRegistry)
        {
            this->toImplementation()->loadModulesImpl(modules, serviceRegistry);
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

    template<typename TServiceRegistry, typename TModuleLoader, typename TLogger>
    class ModulesService: public IModulesService<ModulesService<TServiceRegistry, TModuleLoader, TLogger>>
    {
    private:
        using ModuleType = IModule<TServiceRegistry>;

        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        using ModelLoaderType = IModuleLoader<TModuleLoader>;
        ModelLoaderType& moduleLoader;

        std::map<std::string, ModuleHandle<ModuleType>> moduleHandles;
        std::condition_variable unloadCheck;

        void unload(const std::string& moduleName)
        {
            if(moduleHandles.contains(moduleName))
            {
                auto& handle = moduleHandles[moduleName];
                if(handle.hotReloadEnabled)
                {
                    auto lock = std::lock_guard<std::mutex>{handle.mutex};
                    moduleHandles.erase(moduleName);
                    handle.state = nullptr;
                }
                else
                {
                    moduleHandles.erase(moduleName);
                    handle.state = nullptr;
                }
            }
        }

        void load(const std::string& moduleName, const std::filesystem::path& modulePath, TServiceRegistry& serviceRegistry)
        {
            auto insertion = moduleHandles.emplace(moduleName, ModuleHandle<ModuleType>{ moduleName, modulePath });
            if(insertion.second)
            {
                auto& handle = *insertion.first;
                moduleLoader.template load<ModuleType>(modulePath, moduleName, handle);
                if(handle.state)
                {
                    constexpr auto errorPattern = R"(Error on running module "{0}" from "{1}": {2})";
                    try
                    {
                        //TODO: pass handle by reference, so that subsequent code has access to the state and handle's mutex
                        handle.module->run(handle.state, serviceRegistry);
                        moduleHandles[handle.name] = handle;
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
            }
            else
            {
                logger.warning(fmt::format("Module is loaded already: {0}, {1}", moduleName, modulePath.string()));
            }
        }

        void reload(const std::string& moduleName, const std::string& modulePath, TServiceRegistry& serviceRegistry)
        {
            unload(moduleName);
            load(moduleName, modulePath, serviceRegistry);
        }

    public:
        ModulesService(ModelLoaderType& moduleLoader, LoggerType& logger):
                moduleLoader(moduleLoader),
                logger(logger)
        {}

        void loadModulesImpl(const std::map<std::string, std::string>& modules, TServiceRegistry& serviceRegistry)
        {
            for(auto& pair : modules)
            {
                reload(pair.first, pair.second, serviceRegistry);
            }
        }

        void unloadModuleImpl(const std::string& moduleName)
        {
            unload(moduleName);
        }
    };
}