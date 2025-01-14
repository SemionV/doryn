#include <dory/core/registry.h>
#include <dory/core/devices/imageStreamDevice.h>
#include <thread>
#include <dory/core/resources/imageStreamQueue.h>

namespace dory::core::devices
{
    ImageStreamDevice::ImageStreamDevice(Registry& registry): DependencyResolver(registry)
    {}

    void ImageStreamDevice::connect(resources::DataContext& context)
    {
        stop = false;
        auto thread = std::jthread{ [this] { workingThread(); } };
        thread.detach();
    }

    void ImageStreamDevice::disconnect(resources::DataContext& context)
    {
        stop = true;
        updateStream();
    }

    void ImageStreamDevice::updateStream()
    {
        queueUpdate.notify_one();
    }

    void ImageStreamDevice::workingThread()
    {
        while(!stop)
        {
            auto imageStreamRepo = _registry.get<repositories::IImageStreamRepository>();

            if(imageStreamRepo)
            {
                imageStreamRepo->each([this](resources::entities::ImageStream& stream)
                {
                    auto& streamQueue = static_cast<resources::entities::ImageStreamQueue&>(stream);
                    if(auto imageFileService = _registry.get<services::files::IImageFileService>(streamQueue.fileFormat))
                    {
                        resources::assets::Image image;
                        if(streamQueue.receive(image))
                        {
                            //TODO: use asynchronous method
                            imageFileService->save(stream.destinationDirectory / image.name, image);
                        }
                    }
                });
            }

            std::unique_lock lock { queueMutex };
            queueUpdate.wait(lock, [this] {
                return stop || isWorkAvailable();
            });
        }
    }

    bool ImageStreamDevice::isWorkAvailable() const
    {
        if(auto blockStreamRepo = _registry.get<repositories::IImageStreamRepository>())
        {
            const auto nonEmptyBlockStream = blockStreamRepo->scan([this](resources::entities::ImageStream& stream)
            {
                auto& streamQueue = static_cast<resources::entities::ImageStreamQueue&>(stream);
                return streamQueue.empty();
            });

            return nonEmptyBlockStream != nullptr;
        }

        return false;
    }

}