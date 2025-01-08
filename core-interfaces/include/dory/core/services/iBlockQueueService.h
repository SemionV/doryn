#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/stream.h>

namespace dory::core::services
{
    class IBlockQueueService: public generic::Interface
    {
    public:
          virtual void addBlockToQueue(resources::IdType queueId, const resources::entities::Block& block) = 0;
    };
}