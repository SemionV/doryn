#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include "client/client.h"

namespace testPlugin
{
    class TestPlugin: public client::PluginInterfaceType
    {
    public:
        void initialize(client::Registry& registry) override;
    };
}