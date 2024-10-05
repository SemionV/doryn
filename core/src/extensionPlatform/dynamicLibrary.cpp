#include <dory/core/extensionPlatform/dynamicLibrary.h>

namespace dory::core::extensionPlatform
{
    DynamicLibrary::~DynamicLibrary()
    {
        _modules.clear();
        _dll.unload();
    }

    std::shared_ptr<IExtensionModule> DynamicLibrary::loadModuleInstance(const std::string &moduleName, const resources::ExtensionContext &moduleContext)
    {
        assert(_dll.is_loaded());

        auto moduleFactory = _dll.template get<ExtensionModuleFactory>(std::string{ extensionModuleFactoryFunctionName });
        IExtensionModule* module = moduleFactory(moduleName, moduleContext);
        return std::shared_ptr<IExtensionModule>{module};
    }

    std::shared_ptr<IExecutableModule> DynamicLibrary::loadModuleInstance(const std::string &moduleName, const resources::ExecuteContext& moduleContext)
    {
        assert(_dll.is_loaded());

        auto moduleFactory = _dll.template get<ExecutableModuleFactory>(std::string{ executableModuleFactoryFunctionName });
        IExecutableModule* module = moduleFactory(moduleName, moduleContext);
        return std::shared_ptr<IExecutableModule>{module};
    }

    void DynamicLibrary::load(const std::filesystem::path &libraryPath)
    {
        assert(!_isLoaded);

        auto path = libraryPath.string() + std::string { ILibrary::systemSharedLibraryFileExtension };
        auto flags = boost::dll::load_mode::rtld_local | boost::dll::load_mode::rtld_now;
        _dll.load(path, flags);
        _isLoaded = true;
    }

    void DynamicLibrary::unload()
    {
        assert(_isLoaded);
        _isLoaded = false;
    }

    bool DynamicLibrary::isLoaded()
    {
        return _isLoaded;
    }

    std::shared_ptr<IExtensionModule> DynamicLibrary::loadModule(const std::string &moduleName, const resources::ExtensionContext& moduleContext)
    {
        return loadModule<IExtensionModule, resources::ExtensionContext>(moduleName, moduleContext);
    }

    std::shared_ptr<IExecutableModule>
    DynamicLibrary::loadModule(const std::string &moduleName, const resources::ExecuteContext &moduleContext)
    {
        return loadModule<IExecutableModule, resources::ExecuteContext>(moduleName, moduleContext);
    }
}