#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <dory/core/resources/dataContext.h>
#include <dory/core/resources/id.h>

template<typename TInterface, typename TEntity = typename TInterface::EntityType>
class EntityRepositoryMock: public TInterface
{
protected:
    MOCK_METHOD(dory::core::resources::IdType , getNewId, ());

public:
    MOCK_METHOD(std::size_t, count, ());
    MOCK_METHOD(TEntity*, get, (typename TEntity::IdType id));
    MOCK_METHOD(TEntity*, create, ());
    MOCK_METHOD(TEntity*, insert, (TEntity&& entity));
    MOCK_METHOD(TEntity*, insert, (const TEntity& entity));
    MOCK_METHOD(void, remove, (typename TEntity::IdType id));
    MOCK_METHOD(TEntity*, scan, (std::function<bool(TEntity& entity)> predicate));
    MOCK_METHOD(void, each, (std::function<void(TEntity& entity)> predicate));
    MOCK_METHOD(void, setName, (typename TEntity::IdType id, std::string name));
    MOCK_METHOD(typename TEntity::IdType, getId, (std::string name));
};
