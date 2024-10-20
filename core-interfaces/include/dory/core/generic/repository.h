#pragma once

#include <dory/core/repositories/iRepository.h>
#include <vector>

namespace dory::core::generic::repository
{
    template<typename TEntity, typename TId = resources::entity::IdType, template<class, class> class TContainer = std::vector>
    class Repository: public repositories::IRepository<TEntity, TId>
    {
    private:
        TContainer<TEntity, std::allocator<TEntity>> container;
        TId counter {};

    public:
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

        TEntity store(TEntity&& entity) override
        {
            entity.id = getFreeId();
            return container.emplace_back(std::move(entity));
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
    };
}
