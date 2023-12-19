#include "dependencies.h"
#include "base/base.h"
#include "engine/entity.h"

using namespace dory::domain;

struct TestEntity: public dory::entity::Entity<dory::entity::IdType>
{
    TestEntity(entity::IdType id):
        dory::entity::Entity<dory::entity::IdType>(id)
    {}
};

TEST_CASE( "Get entities count", "[repository]" )
{
    auto repository = std::make_shared<dory::domain::EntityRepository<TestEntity>>(
        std::initializer_list<TestEntity>
        {
            TestEntity(1),
            TestEntity(2)
        });

    REQUIRE(repository->getEntitiesCount() == 2);
}

TEST_CASE( "Iterator", "[repository]" )
{
    auto repository = std::make_shared<dory::domain::EntityRepository<TestEntity>>(
        std::initializer_list<TestEntity>
        {
            TestEntity(1),
            TestEntity(2)
        });

    std::vector<entity::IdType> ids;

    auto iterator = repository->getTraverseIterator();
    iterator.forEach([&ids](const TestEntity& entity)
    {
        ids.push_back(entity.id);
    });

    REQUIRE(ids.size() == 2);
    REQUIRE(ids[0] == 1);
    REQUIRE(ids[1] == 2);
}

TEST_CASE( "Store entity", "[repository]" )
{
    auto repository = std::make_shared<dory::domain::EntityRepository<TestEntity>>();

    auto entity = repository->store(TestEntity(1));

    REQUIRE(entity.id == 1);
    REQUIRE(repository->getEntitiesCount() == 1);
}

TEST_CASE( "Remove", "[repository]" )
{
    auto repository = std::make_shared<dory::domain::EntityRepository<TestEntity>>(
        std::initializer_list<TestEntity>
        {
            TestEntity(1),
            TestEntity(2)
        });

    repository->remove([](const TestEntity& entity)
    {
        return entity.id == 1;
    });

    std::vector<entity::IdType> ids;

    auto iterator = repository->getTraverseIterator();
    iterator.forEach([&ids](const TestEntity& entity)
    {
        ids.push_back(entity.id);
    });

    REQUIRE(ids.size() == 1);
    REQUIRE(ids[0] == 2);
}
