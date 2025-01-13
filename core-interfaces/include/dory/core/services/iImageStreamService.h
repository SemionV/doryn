#pragma once

#include <dory/generic/baseTypes.h>
#include <dory/core/resources/entities/stream.h>

namespace dory::core::services
{
    class IImageStreamService: public generic::Interface
    {
    public:
          virtual void sendImageToStream(resources::IdType queueId, resources::assets::Image&& image) = 0;
    };
}