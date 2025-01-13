#pragma once

#include <dory/core/resources/entities/stream.h>
#include <readerwriterqueue.h>

namespace dory::core::resources::entities
{
    class ImageStreamQueue: public ImageStream
    {
    private:
        moodycamel::ReaderWriterQueue<assets::Image> _queue {};

    public:
        void send(assets::Image&& image) final;
        bool receive(assets::Image& image) final;
        bool empty() final;
    };
}
