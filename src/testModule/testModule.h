#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include "client/client.h"

namespace testModule
{
    class TestModule: public client::ModuleInterfaceType
    {
    public:
        void initialize(client::Registry& registry) override;
    };
}