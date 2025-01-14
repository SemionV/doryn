#pragma once

#include "configuration.h"
#include "localization.h"
#include "id.h"
#include "profiling.h"

namespace dory::core::resources
{
    struct DataContext
    {
        configuration::Configuration& configuration;
        Localization& localization;
        IdType inputGroupNodeId = nullId;
        IdType outputGroupNodeId = nullId;
        IdType mainWindowId = nullId;
        profiling::Profiling profiling {};

        explicit DataContext(configuration::Configuration& configuration, Localization& localization):
            configuration(configuration),
            localization(localization)
        {}
    };
}