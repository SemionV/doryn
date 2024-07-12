#pragma once

#include <dory/engine.h>
#include <dory/bootstrap.h>
#include "mainModule/registry.h"

namespace dory::sandbox
{
    class MainModule: public IExecutableModule<bootstrap::StartupModuleContext>
    {
    private:
        using DataContextType = Registry::DataContextType;

        Registry::ConfigurationType configuration;
        Registry registry;
        Registry::ServiceTypes::FrameServiceType frameService;

    public:
        MainModule();

        int run(bootstrap::StartupModuleContext& moduleContext) final;
    };
}