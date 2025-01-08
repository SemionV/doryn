#pragma once

#include <dory/core/services/iBlockQueueService.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services
{
    class BlockQueueService: public DependencyResolver, public IBlockQueueService
    {
    public:
        explicit BlockQueueService(Registry& registry);

        void addBlockToQueue(resources::IdType queueId, const resources::entities::Block& block) final;
    };
}