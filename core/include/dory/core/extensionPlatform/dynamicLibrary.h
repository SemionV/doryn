#pragma once

#include <boost/dll.hpp>
#include <dory/core/extensionPlatform/iDynamicLibrary.h>
#include <atomic>

namespace dory::core::extensionPlatform
{
    class DynamicLibrary: public IDynamicLibrary
    {
    private:
        constexpr const static std::string_view extensionModuleFactoryFunctionName = "extensionModuleFactory";
        using ExtensionModuleFactory = IExtensionModule*(const std::string& moduleName);

        constexpr const static std::string_view executableModuleFactoryFunctionName = "executableModuleFactory";
        using ExecutableModuleFactory = IExecutableModule*(const std::string& moduleName);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

        template<typename TModuleType, typename TModuleFactory>
        auto loadModule(const std::string& moduleName, TModuleFactory moduleFactory)
        {
            if(_modules.contains(moduleName))
            {
                _modules.erase(moduleName);
            }

            assert(_dll.is_loaded());
            auto module = std::shared_ptr<TModuleType>{moduleFactory(moduleName)};

            if(module)
            {
                _modules[moduleName] = module;
            }

            return module;
        }

    public:
        explicit DynamicLibrary(const std::string &libraryName, const std::filesystem::path &libraryPath)
                : IDynamicLibrary(libraryName, libraryPath)
        {}

        ~DynamicLibrary() override;

        void load(const std::filesystem::path& libraryPath) override;
        void unload() override;
        bool isLoaded() override;
        std::shared_ptr<IExtensionModule> loadExtensionModule(const std::string& moduleName) override;
        std::shared_ptr<IExecutableModule> loadExecutableModule(const std::string& moduleName) override;
    };
}