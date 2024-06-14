#pragma once

#include "base/dependencies.h"
#include "base/module.h"
#include "registry.h"

namespace client
{
    using ModuleInterfaceType = dory::IModule<Registry>;
}