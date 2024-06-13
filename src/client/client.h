#pragma once

#include "base/dependencies.h"
#include "base/plugin.h"
#include "registry.h"

namespace client
{
    using PluginInterfaceType = dory::plugin::Plugin<Registry>;
}