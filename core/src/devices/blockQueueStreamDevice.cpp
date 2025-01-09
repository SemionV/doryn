#include <dory/core/registry.h>
#include <dory/core/devices/blockQueueStreamDevice.h>
#include <thread>
#include <dory/core/resources/blockStreamQueue.h>

namespace dory::core::devices
{
    BlockQueueStreamDevice::BlockQueueStreamDevice(Registry& registry): DependencyResolver(registry)
    {}

    void BlockQueueStreamDevice::connect(resources::DataContext& context)
    {
        stop = false;
        auto thread = std::jthread{ [this] { workingThread(); } };
        thread.detach();
    }

    void BlockQueueStreamDevice::disconnect(resources::DataContext& context)
    {
        stop = true;
        updateStream();
    }

    void BlockQueueStreamDevice::updateStream()
    {
        queueUpdate.notify_one();
    }

    void BlockQueueStreamDevice::workingThread()
    {
        while(!stop)
        {
            if(auto blockStreamRepo = _registry.get<repositories::IBlockStreamRepository>())
            {
                blockStreamRepo->each([this](resources::entities::BlockStream& blockStream)
                {
                    const auto& blockStreamQueue = static_cast<resources::entities::BlockStreamQueue&>(blockStream);

                    const resources::entities::Block* block = blockStreamQueue.queue.peek();
                    while(block != nullptr)
                    {
                        //TODO: store block in the block repository

                        block = blockStreamQueue.queue.peek();
                    }
                });
            }

            std::unique_lock lock {queueMutex};
            queueUpdate.wait(lock, [this] {
                return stop || isWorkAvailable();
            });
        }
    }

    bool BlockQueueStreamDevice::isWorkAvailable() const
    {
        if(auto blockStreamRepo = _registry.get<repositories::IBlockStreamRepository>())
        {
            const auto nonEmptyBlockStream = blockStreamRepo->scan([this](resources::entities::BlockStream& blockStream)
            {
                const auto& blockStreamQueue = static_cast<resources::entities::BlockStreamQueue&>(blockStream);
                return blockStreamQueue.queue.size_approx() > 0;
            });

            return nonEmptyBlockStream != nullptr;
        }

        return false;
    }

}
