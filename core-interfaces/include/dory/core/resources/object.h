#pragma once

#include "entity.h"
#include <list>

namespace dory::core::resources::object
{
    template<typename TDataContext>
    struct PipelineNode
    {

        entity::PipelineNode<TDataContext> nodeEntity;
        std::list<std::shared_ptr<object::PipelineNode<TDataContext>>> children;

        explicit PipelineNode(const entity::PipelineNode<TDataContext>& nodeEntity):
                nodeEntity(nodeEntity)
        {}
    };
}
