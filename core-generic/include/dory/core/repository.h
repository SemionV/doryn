#pragma once

#include <dory/core/repositories/iRepository.h>
#include <map>

namespace dory::core::repositories
{
    template<typename TEntity,
            typename TId = resources::IdType,
            typename TInterface = repositories::IRepository<TEntity, TId>>
    class EntityRepository: public TInterface
    {
    protected:
        std::map<TId, TEntity> container;
        std::unordered_map<std::string, TId> _names;
        TId counter {};

    public:
        EntityRepository() = default;

        template<typename T>
        explicit EntityRepository(T&& entities):
            container(std::forward<T>(entities))
        {}

        std::size_t count() override
        {
            return container.size();
        }

        TId getNewId() override
        {
            return ++counter;
        }

        void remove(TId id) override
        {
            if(container.contains(id))
            {
                container.erase(id);
            }

            for(const auto& pair : _names)
            {
                if(pair.second == id)
                {
                    _names.erase(pair.first);
                    break;
                }
            }
        }

        void setEntityName(TId id, std::string name) override
        {
            _names[name] = id;
        }

        TId getEntityId(std::string name) override
        {
            if(_names.contains(name))
            {
                return _names[name];
            }

            return resources::nullId;
        }
    };

    template<typename TEntity,
            typename TId = resources::IdType,
            typename TInterface = repositories::IRepository<TEntity, TId>>
    class Repository: public EntityRepository<TEntity, TId, TInterface>
    {
    public:
        Repository() = default;

        template<typename T>
        explicit Repository(T&& entities):
            EntityRepository<TEntity, TId, TInterface>(std::forward<T>(entities))
        {}

        TInterface::EntityType* get(TId id) override
        {
            if(this->container.contains(id))
            {
                return &(this->container[id]);
            }

            return {};
        }

        TInterface::EntityType* insert(const TInterface::EntityType& entity) override
        {
            auto id = this->getNewId();
            auto result = this->container.emplace(id, (TEntity&)entity);
            if(result.second)
            {
                typename TInterface::EntityType& newEntity = (*result.first).second;
                newEntity.id = id;
                return &newEntity;
            }

            return nullptr;
        }

        TInterface::EntityType* insert(TInterface::EntityType&& entity) override
        {
            auto id = this->getNewId();
            auto result = this->container.emplace(id, (TEntity)entity);
            if(result.second)
            {
                typename TInterface::EntityType& newEntity = (*result.first).second;
                newEntity.id = id;
                return &newEntity;
            }

            return nullptr;
        }

        void each(std::function<void(typename TInterface::EntityType& entity)> predicate) override
        {
            for(auto& [id, entity] : this->container)
            {
                predicate(entity);
            }
        }

        TInterface::EntityType* scan(std::function<bool(typename TInterface::EntityType& entity)> predicate) override
        {
            for(auto& [id, entity] : this->container)
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
