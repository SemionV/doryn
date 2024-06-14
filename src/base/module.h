#pragma once

#include "dependencies.h"

namespace dory
{
    template<typename TRegistry>
    class IModule
    {
    public:
        virtual ~IModule() = default;

        virtual void run(TRegistry& registry) = 0;
    };

    template<typename TModule>
    using PluginFactory = std::unique_ptr<TModule>();

    const static std::string moduleFactoryFunctionName = "moduleFactory";
}
