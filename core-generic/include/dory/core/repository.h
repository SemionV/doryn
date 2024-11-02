#pragma once

#include <dory/core/repositories/iRepository.h>
#include <vector>

namespace dory::core::repositories
{
    template<typename TEntity,
            typename TId = resources::IdType,
            typename TInterface = repositories::IRepository<TEntity, TId>,
            template<class, class> class TContainer = std::vector>
    class Repository: public TInterface
    {
    protected:
        TContainer<TEntity, std::allocator<TEntity>> container;
        TId counter {};

    public:
        Repository(std::initializer_list<TEntity>&& entities):
            container(std::move(entities))
        {}

        Repository() = default;

        std::size_t count() override
        {
            return container.size();
        }

        TId getFreeId() override
        {
            return ++counter;
        }

        std::optional<TEntity> get(TId id) override
        {
            auto position = std::ranges::find(container, id, &TEntity::id);
            if(position != container.end())
            {
                return *position;
            }

            return {};
        }

        TId insert(const TEntity& entity) override
        {
            TEntity& newEntity = container.emplace_back(std::move(entity));
            newEntity.id = getFreeId();

            return newEntity.id;
        }

        void store(TEntity& entity) override
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

        void remove(TId id) override
        {
            auto position = std::ranges::find(container, id, &TEntity::id);
            if(position != container.end())
            {
                container.erase(position);
            }
        }

        std::span<TEntity> getAll() override
        {
            return std::span<TEntity>{ container };
        }
    };
}
