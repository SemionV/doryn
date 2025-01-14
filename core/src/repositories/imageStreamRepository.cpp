#include <dory/core/repositories/imageStreamRepository.h>

namespace dory::core::repositories
{
    ImageStreamRepository::EntityType* ImageStreamRepository::get(IdType id)
    {
        if(container.contains(id))
        {
            return container[id].get();
        }

        return {};
    }

    ImageStreamRepository::EntityType* ImageStreamRepository::create()
    {
        return insert(resources::entities::ImageStreamQueue {});
    }

    ImageStreamRepository::EntityType* ImageStreamRepository::insert(const resources::entities::ImageStream& stream)
    {
        return insert((resources::entities::ImageStreamQueue&&)stream);
    }

    ImageStreamRepository::EntityType* ImageStreamRepository::insert(resources::entities::ImageStream&& stream)
    {
        auto id = getNewId();

        const auto streamQueue = std::make_shared<resources::entities::ImageStreamQueue>((resources::entities::ImageStreamQueue&&)stream);
        if(streamQueue)
        {
            streamQueue->id = id;
            container[id] = streamQueue;
            return streamQueue.get();
        }

        return nullptr;
    }

    void ImageStreamRepository::each(std::function<void(EntityType&)> predicate)
    {
        for(auto& [id, entity] : container)
        {
            predicate(*entity);
        }
    }

    ImageStreamRepository::EntityType* ImageStreamRepository::scan(std::function<bool(EntityType&)> predicate)
    {
        for(auto& [id, entity] : container)
        {
            if(predicate(*entity))
            {
                return entity.get();
            }
        }

        return nullptr;
    }
}