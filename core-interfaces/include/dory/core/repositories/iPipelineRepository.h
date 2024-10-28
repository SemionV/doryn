#pragma once

#include "iRepository.h"
#include <dory/core/resources/object.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/dataContext.h>
#include "iRepository.h"

namespace dory::core::repositories
{
    class IPipelineRepository
    {
    public:
        virtual ~IPipelineRepository() = default;

        virtual std::span<resources::entity::PipelineNode> getPipelineNodes() = 0;
        virtual bool addNode(const resources::entity::PipelineNode& pipelineNode) = 0;
    };
}