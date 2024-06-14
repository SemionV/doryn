#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::services::module
{
    template<typename TModule>
    struct ModuleHandle
    {
        using StateType = char;

        const std::size_t id {};
        const std::string name;
        boost::dll::shared_library library;
        std::unique_ptr<TModule> module;
        std::shared_ptr<StateType> state;
    };

    template<typename TImplementation>
    class IModuleLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        template<typename TModule>
        ModuleHandle<TModule> load(const std::filesystem::path& modulePath, const std::string& moduleName)
        {
            return this->toImplementation()->template loadImpl<TModule>(modulePath, moduleName);
        }

        template<typename TModule>
        void unload(const ModuleHandle<TModule>& handle)
        {
            this->toImplementation()->template unloadImpl<TModule>(handle);
        }
    };

    template<typename TLogger>
    class ModuleLoader: public IModuleLoader<ModuleLoader<TLogger>>
    {
    private:
        std::atomic<std::size_t> currentId = 0;

        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        template<typename TModule>
        ModuleHandle<TModule> getNewModuleHandle(const std::string& moduleName)
        {
            return ModuleHandle<TModule>{ currentId++, moduleName };
        }

    public:
        explicit ModuleLoader(LoggerType& logger):
            logger(logger)
        {}

        template<typename TModule>
        ModuleHandle<TModule> loadImpl(const std::filesystem::path& modulePath, const std::string& moduleName)
        {
            auto handle = getNewModuleHandle<TModule>(moduleName);

            logger.information(fmt::format(R"(Load module "{0}" from "{1}")", moduleName, modulePath.string()));

            constexpr auto errorPattern = R"(Error on loading an instance of module "{0}" from "{1}": {2})";
            try
            {
                auto& library = handle.library;
                library.load(modulePath.string());

                auto moduleFactory = library.template get<PluginFactory<TModule>>(moduleFactoryFunctionName);
                handle.module = moduleFactory();
                handle.state = std::make_shared<typename ModuleHandle<TModule>::StateType>();
            }
            catch(const std::exception& e)
            {
                logger.error(fmt::format(errorPattern, moduleName, modulePath.string(), e.what()));
            }
            catch(...)
            {
                logger.error(fmt::format(errorPattern, moduleName, modulePath.string(), "unknown exception type"));
            }

            return handle;
        }

        template<typename TModule>
        void unloadImpl(const ModuleHandle<TModule>& handle)
        {

        }
    };

    template<typename TImplementation>
    class IModuleService: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void load(const dory::configuration::Configuration& configuration)
        {
            this->toImplementation()->loadImpl(configuration);
        }
    };

    class ModuleService: public IModuleService<ModuleService>
    {
    public:
        void loadImpl(const dory::configuration::Configuration& configuration)
        {

        }
    };
}