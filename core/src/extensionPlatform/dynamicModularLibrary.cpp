#include <dory/core/extensionPlatform/dynamicModularLibrary.h>

namespace dory::core::extensionPlatform
{
    template<typename TModuleContext>
    DynamicModularLibrary<TModuleContext>::~DynamicModularLibrary()
    {
        _modules.clear();
        _dll.unload();
    }

    template<typename TModuleContext>
    std::shared_ptr<IDynamicModule> DynamicModularLibrary<TModuleContext>::loadModuleInstance(const std::string &moduleName, TModuleContext &moduleContext)
    {
        assert(_dll.is_loaded());

        auto moduleFactory = _dll.template get<LoadableModuleFactory>(std::string{ dynamicModuleFactoryFunctionName });
        IDynamicModule* module = moduleFactory(moduleName, moduleContext);
        return std::shared_ptr<IDynamicModule>{module};
    }

    template<typename TModuleContext>
    void DynamicModularLibrary<TModuleContext>::load(const std::filesystem::path &libraryPath)
    {
        assert(!_isLoaded);

        auto path = libraryPath.string() + std::string { ILibrary::systemSharedLibraryFileExtension };
        auto flags = boost::dll::load_mode::rtld_local | boost::dll::load_mode::rtld_now;
        _dll.load(path, flags);
        _isLoaded = true;
    }

    template<typename TModuleContext>
    void DynamicModularLibrary<TModuleContext>::unload()
    {
        assert(_isLoaded);
        _isLoaded = false;
    }

    template<typename TModuleContext>
    bool DynamicModularLibrary<TModuleContext>::isLoaded()
    {
        return _isLoaded;
    }

    template<typename TModuleContext>
    std::shared_ptr<IDynamicModule>
    DynamicModularLibrary<TModuleContext>::loadModule(const std::string &moduleName, TModuleContext &moduleContext)
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
}