#pragma once

#include <dory/engine.h>
#include <dory/bootstrapper.h>

namespace dory::sandbox
{
    class MainModule: public IExecutableModule<bootstrapper::StartupModuleContext>
    {
        inline int run(bootstrapper::StartupModuleContext& moduleContext) final;
    };
}