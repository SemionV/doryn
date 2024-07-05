#pragma once

#include <dory/engine.h>
#include <dory/bootstrapper.h>

namespace dory::sandbox
{
    class MainModule: public IExecutableModule<bootstrapper::StartupModuleContext>
    {
    public:
        int run(bootstrapper::StartupModuleContext& moduleContext) final;
    };
}