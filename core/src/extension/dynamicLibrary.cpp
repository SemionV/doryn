#include <dory/core/extension/dynamicLibrary.h>

namespace dory::core::extension
{
    DynamicLibrary::DynamicLibrary(const std::string &libraryName, const std::filesystem::path &libraryPath)
            : IDynamicLibrary(libraryName, libraryPath)
    {}

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

    std::shared_ptr<generic::extension::IModule<resources::DataContext>> DynamicLibrary::loadModule(const std::string& moduleName, Registry& registry)
    {
        if(_modules.contains(moduleName))
        {
            _modules.erase(moduleName);
        }

        assert(_dll.is_loaded());
        auto moduleFactory = _dll.template get<ModuleFactory>(std::string{ moduleFactoryFunctionName });
        auto module = std::shared_ptr<generic::extension::IModule<resources::DataContext>>{moduleFactory(moduleName, registry)};

        if(module)
        {
            _modules[moduleName] = module;
        }

        return module;
    }
}