#pragma once

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
#include "client/client.h"

namespace testModule
{
    class TestModule: public dory::ILoadableModule<client::Registry>
    {
    public:
        void attach(std::shared_ptr<dory::ILibrary> library, client::Registry& registry) override;
        void detach(client::Registry& registry) override;
    };
}