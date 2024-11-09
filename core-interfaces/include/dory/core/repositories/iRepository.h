#pragma once

#include <dory/core/resources/entity.h>
#include <dory/generic/baseTypes.h>
#include <optional>
#include <span>
#include <functional>
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
        virtual TEntity* get(TId id) = 0;
        virtual void store(TEntity& entity) = 0;
        virtual TEntity* insert(TEntity&& entity) = 0;
        virtual TId insert(const TEntity& entity) = 0;
        virtual void remove(TId id) = 0;
        virtual bool scan(std::function<bool(TEntity& entity)> predicate) = 0;
        virtual void each(std::function<void(TEntity& entity)> predicate) = 0;
    };
}
