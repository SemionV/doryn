#pragma once

#include "base/dependencies.h"
#include "idFactory.h"

namespace dory::domain
{
    template<typename TEntity, template<typename T> class TContainer = std::vector>
    class EntityRepository
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