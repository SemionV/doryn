#pragma once

#include "entity.h"

namespace dory::domain
{
    struct DataContext
    {
        dory::domain::entity::IdType inputGroupNodeId;
        dory::domain::entity::IdType outputGroupNodeId;
        dory::domain::entity::IdType mainWindowId;

        DataContext():
                inputGroupNodeId(dory::domain::entity::nullId),
                outputGroupNodeId(dory::domain::entity::nullId),
                mainWindowId(dory::domain::entity::nullId)
        {}
    };
}