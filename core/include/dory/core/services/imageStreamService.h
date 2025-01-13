#pragma once

#include <dory/core/services/iImageStreamService.h>
#include <dory/core/dependencyResolver.h>

namespace dory::core::services
{
    class ImageStreamService: public DependencyResolver, public IImageStreamService
    {
    public:
        explicit ImageStreamService(Registry& registry);

        void sendImageToStream(resources::IdType queueId, resources::assets::Image&& image) final;
    };
}