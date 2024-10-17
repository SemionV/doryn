#pragma once

#include <boost/dll.hpp>
#include <dory/core/extensionPlatform/iDynamicLibrary.h>
#include <atomic>

namespace dory::core::extensionPlatform
{
    class DynamicLibrary: public IDynamicLibrary
    {
    private:
        constexpr const static std::string_view moduleFactoryFunctionName = "moduleFactory";
        using ModuleFactory = IModule*(const std::string& moduleName);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

    public:
        explicit DynamicLibrary(const std::string &libraryName, const std::filesystem::path &libraryPath);
        ~DynamicLibrary() override;

        void load(const std::filesystem::path& libraryPath) override;
        void unload() override;
        bool isLoaded() override;
        std::shared_ptr<IModule> loadModule(const std::string& moduleName) override;
    };
}