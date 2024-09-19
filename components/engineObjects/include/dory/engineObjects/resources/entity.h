#pragma once

#include <dory/engineObjects/resources/dataContext.h>
#include <dory/engine/resources/entity.h>

namespace dory::engine::resources::entity
{
    extern template class PipelineNode<DataContextType>;
    using PipelineNodeType = PipelineNode<DataContextType>;
}