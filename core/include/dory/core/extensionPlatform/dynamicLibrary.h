#pragma once

#include <boost/dll.hpp>
#include <dory/core/extensionPlatform/iDynamicLibrary.h>
#include <dory/core/resources/moduleContext.h>
#include <atomic>

namespace dory::core::extensionPlatform
{
    class DynamicLibrary: public IDynamicLibrary
    {
    private:
        constexpr const static std::string_view extensionModuleFactoryFunctionName = "extensionModuleFactory";
        using ExtensionModuleFactory = IExtensionModule*(const std::string& moduleName, const resources::ExtensionContext& moduleContext);

        constexpr const static std::string_view executableModuleFactoryFunctionName = "executableModuleFactory";
        using ExecutableModuleFactory = IExecutableModule*(const std::string& moduleName, const resources::ExecuteContext& moduleContext);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

        std::shared_ptr<IExtensionModule> loadModuleInstance(const std::string& moduleName, const resources::ExtensionContext& moduleContext);
        std::shared_ptr<IExecutableModule> loadModuleInstance(const std::string& moduleName, const resources::ExecuteContext& moduleContext);

        template<typename TModule, typename TModuleContext>
        std::shared_ptr<TModule> loadModule(const std::string& moduleName, const TModuleContext& moduleContext)
        {
            if(_modules.contains(moduleName))
            {
                _modules.erase(moduleName);
            }

            auto module = this->loadModuleInstance(moduleName, moduleContext);

            if(module)
            {
                _modules[moduleName] = module;
            }

            return module;
        }

    public:
        ~DynamicLibrary() override;

        void load(const std::filesystem::path& libraryPath) override;
        void unload() override;
        bool isLoaded() override;
        std::shared_ptr<IExtensionModule> loadModule(const std::string& moduleName, const resources::ExtensionContext& moduleContext) override;
        std::shared_ptr<IExecutableModule> loadModule(const std::string& moduleName, const resources::ExecuteContext& moduleContext) override;
    };
}