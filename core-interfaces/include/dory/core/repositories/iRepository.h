#pragma once

#include <dory/core/resources/entity.h>
#include <optional>

namespace dory::core::repositories
{
    template<typename TEntity, typename TId = resources::IdType>
    class IRepository
    {
    public:
        virtual ~IRepository() = default;

        virtual std::size_t count() = 0;
        virtual TId getFreeId() = 0;
        virtual std::optional<TEntity> get(TId id) = 0;
        virtual void store(TEntity& entity) = 0;
        virtual TId insert(const TEntity& entity) = 0;
        virtual void remove(TId id) = 0;
        virtual std::span<const TEntity> getAll() = 0;
    };
}
