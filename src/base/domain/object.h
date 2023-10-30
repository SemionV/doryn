#pragma once

#include "entity.h"

namespace dory::domain::object
{
    struct PipelineNode
    {

        entity::PipelineNode nodeEntity;
        std::list<std::shared_ptr<object::PipelineNode>> children;

        PipelineNode(const entity::PipelineNode& nodeEntity):
            nodeEntity(nodeEntity)
        {}
    };
}