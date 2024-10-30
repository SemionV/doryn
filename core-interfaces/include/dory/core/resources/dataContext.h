#pragma once

#include "configuration.h"
#include "localization.h"

namespace dory::core::resources
{
    struct DataContext
    {
        configuration::Configuration& configuration;
        Localization& localization;

        explicit DataContext(configuration::Configuration& configuration, Localization& localization):
            configuration(configuration),
            localization(localization)
        {}
    };
}