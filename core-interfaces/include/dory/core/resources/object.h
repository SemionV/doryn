#pragma once

#include "entity.h"
#include <list>

namespace dory::core::resources::object
{
    struct PipelineNode
    {

        entity::PipelineNode nodeEntity;
        std::list<std::shared_ptr<object::PipelineNode>> children;

        explicit PipelineNode(const entity::PipelineNode& nodeEntity):
                nodeEntity(nodeEntity)
        {}
    };
}
