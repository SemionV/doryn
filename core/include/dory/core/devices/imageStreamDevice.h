#pragma once

#include <dory/core/devices/iBlockQueueStreamDevice.h>
#include <dory/core/dependencyResolver.h>
#include <condition_variable>
#include <mutex>

namespace dory::core::devices
{
    class ImageStreamDevice : public DependencyResolver, public IImageStreamDevice
    {
    private:
        std::condition_variable queueUpdate {};
        std::atomic<bool> stop { true };
        std::mutex queueMutex;

        void workingThread();
        bool isWorkAvailable() const;

    public:
        explicit ImageStreamDevice(Registry& registry);

        void connect(resources::DataContext& context) final;
        void disconnect(resources::DataContext& context) final;
        void updateStream() final;
    };
}
