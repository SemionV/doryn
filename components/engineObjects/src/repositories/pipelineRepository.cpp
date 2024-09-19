#include <dory/engineObjects/repositories/pipelineRepository.h>

namespace dory::engine::repositories
{
    template class PipelineRepository<DataContextType, resources::entity::PipelineNodeType>;
}