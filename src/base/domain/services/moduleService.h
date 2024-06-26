#pragma once

#include <utility>

#include "base/dependencies.h"
#include "base/typeComponents.h"
#include "base/module.h"
#include "logService.h"

namespace dory::domain::services::module
{
    template<typename TModuleContext>
    class BoostDynamicLinkLibrary: public DynamicLinkLibrary<TModuleContext>
    {
    private:
        std::atomic<bool> _isLoaded = false;
        boost::dll::shared_library _dll;
        std::shared_ptr<ILoadableModule<TModuleContext>> _module;

    public:
        bool isLoaded() override
        {
            return _isLoaded;
        }

        std::shared_ptr<ILoadableModule<TModuleContext>> unload() override
        {
            _isLoaded = false;

            auto tmpModule = _module;
            _module = nullptr;

            return tmpModule;
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

        void unload(const std::string& moduleName, TModuleContext& moduleContext)
        {
            this->toImplementation()->unloadImpl(moduleName, moduleContext);
        }
    };

    template<typename TModuleContext, typename TLogger, typename TLibrary = BoostDynamicLinkLibrary<TModuleContext>>
    class ModuleService: public IModuleService<TModuleContext, ModuleService<TModuleContext, TLogger, TLibrary>>
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
            unloadImpl(moduleName, moduleContext);

            logger.information(fmt::format(R"(Load module "{0}" from "{1} library")", moduleName, libraryPath.string()));

            constexpr auto errorPattern = R"(Error on loading module "{0}" from "{1}": {2})";
            try
            {
                std::shared_ptr<DynamicLinkLibrary<TModuleContext>> library = std::make_shared<TLibrary>();
                auto module = library->load(libraryPath);

                if(module)
                {
                    try
                    {
                        module->attach(library, moduleContext);
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

        void unloadImpl(const std::string& moduleName, TModuleContext& moduleContext)
        {
            if(libraries.contains(moduleName))
            {
                auto library = libraries[moduleName];
                auto module = library->unload();
                if(module)
                {
                    module->detach(moduleContext);
                }
                libraries.erase(moduleName);
            }
        }
    };

    template<typename TImplementation>
    class IDynamicLibraryService: NonCopyable
    {
    private:
        TImplementation& _implementation;

    public:
        explicit IDynamicLibraryService(TImplementation& implementation):
            _implementation(implementation)
        {}

        std::shared_ptr<DynamicLibrary> load(const std::filesystem::path& libraryName, const std::filesystem::path& libraryPath)
        {
            return _implementation.load(libraryName, libraryPath);
        }

        void unload(const std::string& libraryName)
        {
            _implementation.unload(libraryName);
        }
    };

    class DynamicLibraryService
    {
    private:
        std::map<std::string, std::shared_ptr<DynamicLibrary>> _libraries;

    public:
        std::shared_ptr<DynamicLibrary> load(const std::string& libraryName, const std::filesystem::path& libraryPath)
        {
            if(_libraries.contains(libraryName))
            {
                return _libraries[libraryName];
            }

            auto library = std::make_shared<DynamicLibrary>(libraryName, libraryPath);
            library->load(libraryPath);

            _libraries[libraryName] = library;

            return library;
        }

        void unload(const std::string& libraryName)
        {
            if(_libraries.contains(libraryName))
            {
                auto library = _libraries[libraryName];
                library->unload();

                _libraries.erase(libraryName);
            }
        }
    };
}