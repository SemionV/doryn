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
        std::unique_ptr<IModule<TServiceRegistry>> moduleInterface;
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

    public:
        explicit ModuleLoader(LoggerType& logger):
            logger(logger)
        {}

        ModuleHandle<TServiceRegistry> loadImpl(const std::filesystem::path& modulePath, const std::string& moduleName, TServiceRegistry& serviceRegistry)
        {
            auto handle = ModuleHandle<TServiceRegistry>{ currentId++, moduleName };

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