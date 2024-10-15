#pragma once

#include <dory/core/iRegistryFactory.h>
#include <dory/core/macros.h>

namespace dory::game::engine
{
    class DORY_DLLEXPORT RegistryFactory: public core::IRegistryFactory
    {
    public:
        std::shared_ptr<core::Registry> createRegistry() override;
    };
}