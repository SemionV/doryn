#pragma once

#include "dependencies.h"

namespace dory::plugin
{
    template<typename TRegistry>
    class Plugin
    {
    public:
        virtual ~Plugin() = default;

        virtual void initialize(TRegistry& registry) = 0;
    };

    template<typename TRegistry>
    using PluginFactory = std::unique_ptr<Plugin<TRegistry>>();
}
