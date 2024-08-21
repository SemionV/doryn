#pragma once

#include "entity.h"
#include "resources/localization.h"
#include "configuration.h"

namespace dory::domain
{
    struct DataContext
    {
        dory::domain::entity::IdType inputGroupNodeId;
        dory::domain::entity::IdType outputGroupNodeId;
        dory::domain::entity::IdType mainWindowId;
        resources::Localization localization;
        configuration::Configuration configuration;

        DataContext():
                inputGroupNodeId(dory::domain::entity::nullId),
                outputGroupNodeId(dory::domain::entity::nullId),
                mainWindowId(dory::domain::entity::nullId)
        {}
    };
}