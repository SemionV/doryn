#pragma once

#include "entity.h"

namespace dory::domain::object
{
    struct PipelineGroup
    {
        entity::PipelineGroup* groupEntity;
        std::list<entity::PipelineNode*> nodeEntities;
    };

    struct Pipeline
    {
        std::list<std::shared_ptr<PipelineGroup>> groups;
    };
}