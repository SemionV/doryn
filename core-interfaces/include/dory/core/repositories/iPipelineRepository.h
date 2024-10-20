#pragma once

#include "iRepository.h"
#include <dory/core/resources/object.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::repositories
{
    class IPipelineRepository: public IRepository<resources::entity::PipelineNode<resources::DataContext>>
    {
    public:
        std::list<std::shared_ptr<resources::object::PipelineNode<resources::DataContext>>> getPipeline();
    };
}
