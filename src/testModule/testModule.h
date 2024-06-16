#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include "client/client.h"

namespace testModule
{
    class TestModule: public dory::ILoadableModule<client::Registry>
    {
    public:
        ~TestModule() override;

        void load(const dory::ModuleHandle& moduleHandle, client::Registry& registry) override;
    };
}