#pragma once

#include "module.h"

namespace dory::core::extensionPlatform
{
    template<typename TModuleContext>
    class IModularLibrary: public ILibrary
    {
    private:
        std::unordered_map<std::string, std::shared_ptr<IModule>> _modules;

    protected:
        virtual IDynamicModule loadModuleInstance(const std::string moduleName, TModuleContext& moduleContext) = 0;

    public:
        explicit IModularLibrary(const std::string& libraryName, const std::filesystem::path& libraryPath):
                ILibrary(libraryName, libraryPath)
        {}

        void unloadModules()
        {
            _modules.clear();
        }

        std::shared_ptr<IDynamicModule> loadModule(const std::string moduleName, TModuleContext& moduleContext)
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
    };
}