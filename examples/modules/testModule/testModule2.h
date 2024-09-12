#pragma once

#include <iostream>

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include <dory/client/client.h>
#include <dory/module.h>

namespace testModule
{
    class TestModule2: public dory::IDynamicModule<client::Registry>
    {
    private:
        client::Registry& _registry;

    public:
        TestModule2(client::Registry& registry);
        ~TestModule2() override;

        inline void attach(dory::LibraryHandle library) final;
    };
}