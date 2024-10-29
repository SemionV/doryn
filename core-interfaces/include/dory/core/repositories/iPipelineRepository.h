#pragma once

#include "iRepository.h"
#include <dory/core/resources/object.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/dataContext.h>
#include <dory/generic/baseTypes.h>
#include "iRepository.h"

namespace dory::core::repositories
{
    class IPipelineRepository: public generic::Interface
    {
    public:
        using EntityType = resources::entity::PipelineNode;
        using IdType = EntityType::IdType;

        virtual std::span<EntityType> getPipelineNodes() = 0;
        virtual IdType addNode(const EntityType& pipelineNode) = 0;
        virtual IdType removeNode(IdType id) = 0;
    };
}