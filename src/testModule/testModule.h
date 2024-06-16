#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include "client/client.h"

namespace testModule
{
    class TestModule: public dory::IModule<client::Registry>
    {
    public:
        ~TestModule() override;

        void run(const dory::ModuleHandle& moduleHandle, client::Registry& registry) override;
    };
}