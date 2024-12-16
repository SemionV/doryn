#pragma once

#include <dory/core/repositories/iRepository.h>
#include <map>

namespace dory::core::repositories
{
    template<typename TEntity,
            typename TId = resources::IdType,
            typename TInterface = repositories::IRepository<TEntity, TId>>
    class Repository: public TInterface
    {
    protected:
        std::map<TId, TEntity> container;
        TId counter {};

    public:
        template<typename T>
        explicit Repository(T&& entities):
            container(std::forward<T>(entities))
        {}

        Repository() = default;

        std::size_t count() override
        {
            return container.size();
        }

        TId getNewId() override
        {
            return ++counter;
        }

        TInterface::EntityType* get(TId id) override
        {
            if(container.contains(id))
            {
                return &(container[id]);
            }

            return {};
        }

        TId insert(const TInterface::EntityType& entity) override
        {
            auto id = getNewId();
            typename TInterface::EntityType& newEntity = container[id] = (TEntity&)entity;
            newEntity.id = id;
            return id;
        }

        TInterface::EntityType* insert(TInterface::EntityType&& entity) override
        {
            auto id = getNewId();
            typename TInterface::EntityType& newEntity = container[id] = (TEntity)entity;
            newEntity.id = id;
            return &newEntity;
        }

        void store(TInterface::EntityType& entity) override
        {
            container[entity.id] = (TEntity&)entity;
        }

        void remove(TId id) override
        {
            if(container.contains(id))
            {
                container.erase(id);
            }
        }

        void each(std::function<void(typename TInterface::EntityType& entity)> predicate) override
        {
            for(auto& [id, entity] : container)
            {
                predicate(entity);
            }
        }

        TInterface::EntityType* scan(std::function<bool(typename TInterface::EntityType& entity)> predicate) override
        {
            for(auto& [id, entity] : container)
            {
                if(predicate(entity))
                {
                    return &entity;
                }
            }

            return nullptr;
        }
    };
}
