#pragma once

#include <boost/dll.hpp>
#include <atomic>
#include <dory/core/extension/iDynamicLibrary.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::extension
{
    class DynamicLibrary: public IDynamicLibrary
    {
    private:
        constexpr const static std::string_view moduleFactoryFunctionName = "moduleFactory";
        using ModuleFactory = dory::generic::extension::IModule<resources::DataContext>*(const std::string& moduleName, Registry& registry);

        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::unordered_map<std::string, std::shared_ptr<dory::generic::extension::IModule<resources::DataContext>>> _modules;

    public:
        explicit DynamicLibrary(const std::string &libraryName, const std::filesystem::path &libraryPath);
        ~DynamicLibrary() override;

        void load(const std::filesystem::path& libraryPath) override;
        void unload() override;
        bool isLoaded() override;
        std::shared_ptr<dory::generic::extension::IModule<resources::DataContext>> loadModule(const std::string& moduleName, Registry& registry) override;
    };
}