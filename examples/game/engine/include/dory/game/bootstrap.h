#pragma once

#include <dory/core/registry.h>
#include <dory/core/iBootstrap.h>
#include <dory/core/resources/localizationImpl.h>
#include <dory/core/resources/configuration.h>

namespace dory::game
{
    class Bootstrap: public core::IBootstrap
    {
    private:
        core::resources::LocalizationImpl _localization;

    public:
        bool initialize(const dory::generic::extension::LibraryHandle& libraryHandle, core::resources::DataContext& context, core::Registry& registry) final;
        bool run(core::resources::DataContext& context, core::Registry& registry) final;
    };
}
