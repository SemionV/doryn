#include <dory/core/resources/blockStreamQueue.h>
#include <dory/core/services/blockQueueService.h>

#include "../../../components/testing/include/dory/testing/messaging_book.h"
#include "dory/core/registry.h"

namespace dory::core::services
{
    BlockQueueService::BlockQueueService(Registry& registry): DependencyResolver(registry)
    {}

    void BlockQueueService::addBlockToQueue(const resources::IdType queueId, const resources::entities::Block& block)
    {
        auto blockStreamRepo = _registry.get<repositories::IBlockStreamRepository>();
        auto streamingDevice = _registry.get<devices::IBlockQueueStreamDevice>();
        if(blockStreamRepo && streamingDevice)
        {
            if(auto* stream = blockStreamRepo->get(queueId))
            {
                auto* queue = (resources::entities::BlockStreamQueue*)stream;
                queue->queue.enqueue(block);

                streamingDevice->updateStream();
            }
        }
    }
}
