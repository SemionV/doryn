#pragma once

#include "entity.h"
#include "resources/localization.h"
#include "configuration.h"

namespace dory::domain
{
    template<typename... TCustomSections>
    struct DataContext: public TCustomSections...
    {
        entity::IdType inputGroupNodeId;
        entity::IdType outputGroupNodeId;
        entity::IdType mainWindowId;
        resources::Localization localization;
        configuration::Configuration configuration;

        DataContext():
                inputGroupNodeId(entity::nullId),
                outputGroupNodeId(entity::nullId),
                mainWindowId(entity::nullId)
        {}
    };
}