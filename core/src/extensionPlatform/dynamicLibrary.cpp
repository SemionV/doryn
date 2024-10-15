#include <dory/core/extensionPlatform/dynamicLibrary.h>

namespace dory::core::extensionPlatform
{
    DynamicLibrary::~DynamicLibrary()
    {
        _modules.clear();
        _dll.unload();
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

    std::shared_ptr<IExtensionModule> DynamicLibrary::loadExtensionModule(const std::string& moduleName)
    {
        auto moduleFactory = _dll.template get<ExtensionModuleFactory>(std::string{ extensionModuleFactoryFunctionName });
        return loadModule<IExtensionModule>(moduleName, moduleFactory);
    }

    std::shared_ptr<IExecutableModule> DynamicLibrary::loadExecutableModule(const std::string& moduleName)
    {
        auto moduleFactory = _dll.template get<ExecutableModuleFactory>(std::string{ executableModuleFactoryFunctionName });
        return loadModule<IExecutableModule>(moduleName, moduleFactory);
    }
}