#pragma once

#include <list>

#include "dory/engine/resources/entity.h"

namespace dory::domain::object
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