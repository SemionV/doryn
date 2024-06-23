#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include "client/registry.h"

namespace testModule
{
    class TestModule2: public dory::IDynamicModule<client::Registry>
    {
    public:
        virtual ~TestModule2();

        inline void attach(dory::LibraryHandle library, client::Registry& registry) final;
    };
}