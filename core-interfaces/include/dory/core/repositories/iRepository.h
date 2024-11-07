#pragma once

#include <dory/core/resources/entity.h>
#include <dory/generic/baseTypes.h>
#include <optional>
#include <span>
#include <dory/generic/typeTraits.h>
#include <dory/generic/span.h>

namespace dory::core::repositories
{
    template<typename TEntity, typename TId = resources::IdType>
    class IRepository: public generic::Interface
    {
    public:
        using EntityType = TEntity;

        virtual std::size_t count() = 0;
        virtual void setId(TEntity& entity) = 0;
        virtual generic::OptionalReference<TEntity> get(TId id) = 0;
        virtual void store(TEntity& entity) = 0;
        virtual TId insert(const TEntity& entity) = 0;
        virtual void remove(TId id) = 0;
        virtual generic::Span<TEntity> getAll() = 0;
    };
}
