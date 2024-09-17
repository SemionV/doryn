#pragma once

#include "entity.h"
#include "dory/engine/resources/localization.h"
#include "dory/engine/resources/configuration.h"

namespace dory::engine::resources
{
    template<typename... TCustomSections>
    struct DataContext: public TCustomSections...
    {
        entity::IdType inputGroupNodeId;
        entity::IdType outputGroupNodeId;
        entity::IdType mainWindowId;
        localization::Localization localization;
        configuration::Configuration configuration;

        DataContext():
                inputGroupNodeId(entity::nullId),
                outputGroupNodeId(entity::nullId),
                mainWindowId(entity::nullId)
        {}
    };
}