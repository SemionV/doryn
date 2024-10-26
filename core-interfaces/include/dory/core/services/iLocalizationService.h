#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/localization.h>
#include <dory/core/resources/configuration.h>

namespace dory::core::services
{
    class ILocalizationService: public generic::Interface
    {
    public:
        virtual void load(const resources::configuration::Configuration& configuration, resources::Localization& localization) = 0;
    };
}
