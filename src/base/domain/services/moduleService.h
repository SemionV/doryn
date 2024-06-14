#pragma once

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::services::module
{
    template<typename TServiceRegistry>
    struct ModuleHandle
    {
        const std::size_t id {};
        const std::string name;
        boost::dll::shared_library library;
        std::unique_ptr<IModule<TServiceRegistry>> moduleInterface;
        std::shared_ptr<char> state;
    };

    template<typename TServiceRegistry, typename TImplementation>
    class IModuleLoader: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        ModuleHandle<TServiceRegistry> load(const std::filesystem::path& modulePath, const std::string& moduleName, TServiceRegistry& serviceRegistry)
        {
            return this->toImplementation()->loadImpl(modulePath, moduleName, serviceRegistry);
        }

        void unload(const ModuleHandle<TServiceRegistry>& handle)
        {
            this->toImplementation()->unloadImpl(handle);
        }
    };

    template<typename TServiceRegistry, typename TLogger>
    class ModuleLoader: public IModuleLoader<TServiceRegistry, ModuleLoader<TServiceRegistry, TLogger>>
    {
    private:
        std::atomic<std::size_t> currentId = 0;

        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        ModuleHandle<TServiceRegistry> getNewModuleHandle(const std::string& moduleName)
        {
            return ModuleHandle<TServiceRegistry>{ currentId++, moduleName };
        }

    public:
        explicit ModuleLoader(LoggerType& logger):
            logger(logger)
        {}

        ModuleHandle<TServiceRegistry> loadImpl(const std::filesystem::path& modulePath, const std::string& moduleName, TServiceRegistry& serviceRegistry)
        {
            auto handle = getNewModuleHandle(moduleName);

            logger.information(fmt::format("Load module {0}, {1}", moduleName, modulePath.string()));

            try
            {

            }
            catch(const std::exception& e)
            {
                logger.error(fmt::format("Error on loading an instance of module {0}, {1}: {2}", moduleName, modulePath.string(), e.what()));
            }
            catch(...)
            {

            }

            return handle;
        }

        void unloadImpl(const ModuleHandle<TServiceRegistry>& handle)
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