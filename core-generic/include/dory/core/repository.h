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

        void setId(TInterface::EntityType& entity) override
        {
            entity.id = ++counter;
        }

        generic::OptionalReference<typename TInterface::EntityType> get(TId id) override
        {
            auto position = std::ranges::find(container, id, &TInterface::EntityType::id);
            if(position != container.end())
            {
                return *position;
            }

            return {};
        }

        TId insert(const TInterface::EntityType& entity) override
        {
            typename TInterface::EntityType& newEntity = container.emplace_back((TEntity&)entity);
            setId(newEntity);

            return newEntity.id;
        }

        void store(TInterface::EntityType& entity) override
        {
            auto position = std::ranges::find(container, entity.id, &std::remove_reference_t<typename TInterface::EntityType>::id);
            if(position != container.end()) //update
            {
                *position = (TEntity&)entity;
            }
            else //create
            {
                container.push_back((TEntity&)entity);
            }
        }

        void remove(TId id) override
        {
            auto position = std::ranges::find(container, id, &TInterface::EntityType::id);
            if(position != container.end())
            {
                container.erase(position);
            }
        }

        generic::Span<typename TInterface::EntityType> getAll() override
        {
            return generic::Span<typename TInterface::EntityType>{ container.data(), container.size(), sizeof(TEntity) };
        }
    };
}
