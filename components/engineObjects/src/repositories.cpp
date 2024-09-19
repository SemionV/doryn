#include <dory/engineObjects/repositories.h>

namespace dory::engine::repositories
{
    template class EntityRepository<resources::entity::Camera>;
    template class EntityRepository<resources::entity::View>;
    template class EntityRepository<resources::opengl::GlfwWindow>;
    template class PipelineRepository<DataContextType, resources::entity::PipelineNode<DataContextType>>;
}