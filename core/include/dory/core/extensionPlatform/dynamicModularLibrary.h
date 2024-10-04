#pragma once

#include <boost/dll.hpp>
#include <dory/core/extensionPlatform/iDynamicModularLibrary.h>
#include <atomic>

namespace dory::core::extensionPlatform
{
    template<typename TModuleContext>
    class DynamicModularLibrary: IDynamicModularLibrary<TModuleContext>
    {
    private:
        constexpr const static std::string_view dynamicModuleFactoryFunctionName = "dynamicModuleFactory";
        using LoadableModuleFactory = IDynamicModule*(const std::string& moduleName, TModuleContext& moduleContext);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

        std::shared_ptr<IDynamicModule> loadModuleInstance(const std::string& moduleName, TModuleContext& moduleContext);

    public:
        ~DynamicModularLibrary() override;

        void load(const std::filesystem::path& libraryPath) override;
        void unload() override;
        bool isLoaded() override;
        std::shared_ptr<IDynamicModule> loadModule(const std::string& moduleName, TModuleContext& moduleContext) override;
    };
}