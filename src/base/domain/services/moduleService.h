#pragma once

#include <utility>

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"

namespace dory::domain::services::module
{
    template<typename TModuleContext>
    class BoostDynamicLinkLibrary: public DynamicLinkLibrary<TModuleContext>
    {
    private:
        bool _isLoaded = false;
        boost::dll::shared_library _dll;
        std::shared_ptr<ILoadableModule<TModuleContext>> _module;

    public:
        bool isLoaded() override
        {
            return _isLoaded;
        }

        void unload() override
        {
            _isLoaded = false;
            _module = nullptr;
        }

        std::shared_ptr<ILoadableModule<TModuleContext>> load(const std::filesystem::path& libraryPath) override
        {
            auto path = libraryPath.string() + std::string { ILibrary::systemSharedLibraryFileExtension };
            _dll.load(path);

            auto moduleFactory = _dll.template get<LoadableModuleFactory<TModuleContext>>(loadableModuleFactoryFunctionName);
            _module = moduleFactory();

            if(_module)
            {
                _isLoaded = true;
                return _module;
            }

            return nullptr;
        }
    };

    template<typename TModuleContext, typename TImplementation>
    class IModuleService: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void load(const std::string& moduleName, const std::filesystem::path& modulePath, TModuleContext& moduleContext)
        {
            this->toImplementation()->loadImpl(moduleName, modulePath, moduleContext);
        }

        void unload(const std::string& moduleName)
        {
            this->toImplementation()->unloadImpl(moduleName);
        }
    };

    template<typename TModuleContext, typename TLogger>
    class ModuleService: public IModuleService<TModuleContext, ModuleService<TModuleContext, TLogger>>
    {
    private:
        using LoggerType = ILogService<TLogger>;
        LoggerType& logger;

        std::map<std::string, std::shared_ptr<DynamicLinkLibrary<TModuleContext>>> libraries;

    public:
        explicit ModuleService(LoggerType& logger):
            logger(logger)
        {}

        void loadImpl(const std::string& moduleName, const std::filesystem::path& libraryPath, TModuleContext& moduleContext)
        {
            unloadImpl(moduleName);

            logger.information(fmt::format(R"(Load module "{0}" from "{1} library")", moduleName, libraryPath.string()));

            constexpr auto errorPattern = R"(Error on loading module "{0}" from "{1}": {2})";
            try
            {
                auto library = std::make_shared<BoostDynamicLinkLibrary<TModuleContext>>();
                auto module = library->load(libraryPath);

                if(module)
                {
                    try
                    {
                        module->initialize(library, moduleContext);
                        libraries[moduleName] = library;
                    }
                    catch(...)
                    {
                        library->unload();
                        throw;
                    }
                }
            }
            catch(const std::exception& e)
            {
                logger.error(fmt::format(errorPattern, moduleName, libraryPath.string(), e.what()));
            }
            catch(...)
            {
                logger.error(fmt::format(errorPattern, moduleName, libraryPath.string(), "unknown exception type"));
            }
        }

        void unloadImpl(const std::string& moduleName)
        {
            if(libraries.contains(moduleName))
            {
                auto library = libraries[moduleName];
                library->unload();
                libraries.erase(moduleName);
            }
        }
    };
}