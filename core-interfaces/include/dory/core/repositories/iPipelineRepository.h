#pragma once

#include "iRepository.h"
#include <dory/core/resources/object.h>
#include <dory/core/resources/dataContext.h>

namespace dory::core::repositories
{
    class IPipelineRepository
    {
    public:
        virtual ~IPipelineRepository() = default;
        virtual std::list<std::shared_ptr<resources::object::PipelineNode>> getPipeline() = 0;
    };
}