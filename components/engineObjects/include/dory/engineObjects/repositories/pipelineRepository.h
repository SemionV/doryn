#pragma once

#include <dory/engineObjects/resources/dataContext.h>
#include <dory/engineObjects/resources/entity.h>
#include <dory/engine/repositories/pipelineRepository.h>

namespace dory::engine::repositories
{
    extern template class PipelineRepository<DataContextType, resources::entity::PipelineNodeType>;
    using PipelineRepositoryType = PipelineRepository<DataContextType, resources::entity::PipelineNodeType>;
}