#include <dory/core/resources/imageStreamQueue.h>

namespace dory::core::resources
{
    void entities::ImageStreamQueue::send(assets::Image&& image)
    {
        _queue.enqueue(std::move(image));
    }

    bool entities::ImageStreamQueue::receive(assets::Image& image)
    {
        return _queue.try_dequeue(image);
    }

    bool entities::ImageStreamQueue::empty()
    {
        return _queue.size_approx() > 0;
    }
}
