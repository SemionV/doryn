#include <dory/core/resources/assets/image.h>
#include <dory/core/resources/imageStreamQueue.h>
#include <dory/core/services/imageStreamService.h>

#include "dory/core/registry.h"

namespace dory::core::services
{
    ImageStreamService::ImageStreamService(Registry& registry): DependencyResolver(registry)
    {}

    void ImageStreamService::sendImageToStream(resources::IdType streamId, resources::assets::Image&& image)
    {
        auto imageStreamRepo = _registry.get<repositories::IImageStreamRepository>();
        auto streamingDevice = _registry.get<devices::IImageStreamDevice>();
        if(imageStreamRepo && streamingDevice)
        {
            if(auto* stream = imageStreamRepo->get(streamId))
            {
                auto* queueStream = static_cast<resources::entities::ImageStreamQueue*>(stream);
                queueStream->send(std::move(image));

                streamingDevice->updateStream();
            }
        }
    }
}
