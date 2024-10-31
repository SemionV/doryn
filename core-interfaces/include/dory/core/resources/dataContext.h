#pragma once

#include "configuration.h"
#include "localization.h"
#include "id.h"

namespace dory::core::resources
{
    struct DataContext
    {
        configuration::Configuration& configuration;
        Localization& localization;
        IdType inputGroupNodeId = nullId;
        IdType outputGroupNodeId = nullId;
        IdType mainWindowId = nullId;

        explicit DataContext(configuration::Configuration& configuration, Localization& localization):
            configuration(configuration),
            localization(localization)
        {}
    };
}