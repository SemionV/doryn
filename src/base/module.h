#pragma once

#include "dependencies.h"

namespace dory
{
    using ModuleStateBasicType = char;
    using ModuleStateReferenceType = std::weak_ptr<ModuleStateBasicType>;
    using ModuleStateType = std::shared_ptr<ModuleStateBasicType>;

    template<typename TModule>
    using ModuleFactory = std::unique_ptr<TModule>();

    const static std::string moduleFactoryFunctionName = "moduleFactory";
#ifdef WIN32
    const static std::string& systemSharedLibraryFileExtension = ".dll";
#endif

#ifdef __unix__
    const static std::string& systemSharedLibraryFileExtension = ".so";
#endif

    template<typename TRegistry>
    class IModule
    {
    public:
        virtual ~IModule() = default;

        virtual void run(ModuleStateType moduleState, TRegistry& registry) = 0;
    };
}
