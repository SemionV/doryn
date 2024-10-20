#pragma once

#include <dory/generics/typeTraits.h>
#include <dory/engine/resources/entity.h>

namespace dory::engine::repositories
{
    template<typename TImplementation, typename TEntity, typename TId = resources::entity::IdType>
    class IEntityRepository: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        std::size_t count()
        {
            return this->toImplementation()->countImpl();
        }

        TId getFreeId()
        {
            return this->toImplementation()->getFreeIdImpl();
        }

        std::optional<TEntity> get(TId id)
        {
            return this->toImplementation()->getImpl(id);
        }

        TEntity store(TEntity&& entity)
        {
            return this->toImplementation()->storeImpl(std::move(entity));
        }

        void store(TEntity& entity)
        {
            this->toImplementation()->storeImpl(entity);
        }

        void remove(TId id)
        {
            this->toImplementation()->removeImpl(id);
        }

        template<typename F>
        void forEach(F&& predicate)
        {
            this->toImplementation()->forEachImpl(std::forward<F>(predicate));
        }

        template<typename F>
        std::optional<TEntity> find(F&& predicate)
        {
            return this->toImplementation()->findImpl(std::forward<F>(predicate));
        }
    };

    template<typename TEntity, typename TId = resources::entity::IdType, template<class, class> class TContainer = std::vector>
    class EntityRepository: public IEntityRepository<EntityRepository<TEntity, TId, TContainer>, TEntity, TId>
    {
    private:
        TContainer<TEntity, std::allocator<TEntity>> container;
        TId counter;

    public:
        EntityRepository() = default;

        EntityRepository(std::initializer_list<TEntity>&& entities):
                container(std::move(entities))
        {}

        TId getFreeIdImpl()
        {
            return ++counter;
        }

        std::size_t countImpl()
        {
            return container.size();
        }

        std::optional<TEntity> getImpl(TId id)
        {
            auto position = std::ranges::find(container, id, &TEntity::id);
            if(position != container.end())
            {
                return *position;
            }

            return {};
        }

        TEntity storeImpl(TEntity&& entity)
        {
            entity.id = getFreeIdImpl();
            return container.emplace_back(std::move(entity));
        }

        void storeImpl(TEntity& entity)
        {
            auto position = std::ranges::find(container, entity.id, &std::remove_reference_t<TEntity>::id);
            if(position != container.end()) //update
            {
                *position = entity;
            }
            else //create
            {
                container.push_back(entity);
            }
        }

        void removeImpl(TId id)
        {
            auto position = std::ranges::find(container, id, &TEntity::id);
            if(position != container.end())
            {
                container.erase(position);
            }
        }

        template<typename F>
        void forEachImpl(F&& functor)
        {
            std::ranges::for_each(container, std::forward<F>(functor));
        }

        template<typename F>
        std::optional<TEntity> findImpl(F&& predicate)
        {
            auto position = std::ranges::find_if(container, predicate);
            if(position != container.end())
            {
                return *position;
            }

            return {};
        }
    };
}