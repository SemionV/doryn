#pragma once

#include "base/dependencies.h"
#include "idFactory.h"
#include "base/typeComponents.h"

namespace dory::domain
{
    template<typename TImpelementation, typename TEntity, typename TId>
    class IEntityRepository: Uncopyable, public StaticInterface<TImpelementation>
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
            return this->toImplementation()->getImpl();
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

    template<typename TEntity, typename TId, template<typename T> class TContainer = std::vector>
    class EntityRepository2: public IEntityRepository<EntityRepository2<TEntity, TId, TContainer>, TEntity, TId>
    {
    private:
        TContainer<TEntity> container;
        TId counter;

    public:
        EntityRepository2() = default;

        EntityRepository2(std::initializer_list<TEntity>&& entities):
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

    template<typename TEntity, template<typename T> class TContainer = std::vector>
    class EntityRepository: Uncopyable
    {
    private:
        TContainer<TEntity> container;

    public:
        EntityRepository() = default;

        EntityRepository(std::initializer_list<TEntity>&& entities):
            container(std::move(entities))
        {}

        std::size_t count()
        {
            return std::size(container);
        }

        template<class TId>
        std::optional<TEntity> get(TId id)
        {
            auto position = std::ranges::find(container, id, &TEntity::id);
            if(position != container.end())
            {
                return *position;
            }

            return {};
        }

        TEntity store(TEntity&& entity)
        {
            return container.emplace_back(std::move(entity));
        }

        void store(TEntity& entity)
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

        template<class TId>
        void remove(TId id)
        {
            auto position = std::ranges::find(container, id, &TEntity::id);
            if(position != container.end())
            {
                container.erase(position);
            }
        }

        template<typename F>
        void forEach(F&& functor)
        {
            std::ranges::for_each(container, std::forward<F>(functor));
        }

        template<typename F>
        std::optional<TEntity> find(F&& predicate)
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