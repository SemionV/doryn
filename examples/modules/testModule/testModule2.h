#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include <dory/client/client.h>

namespace testModule
{
    class TestModule2: public dory::IDynamicModule<client::Registry>
    {
    public:
        ~TestModule2() override;

        inline void attach(dory::LibraryHandle library, client::Registry& registry) final;
    };
}