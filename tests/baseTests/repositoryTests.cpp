#include <dory/engine/domain/entity.h>
#include <dory/engine/domain/entityRepository.h>

using namespace dory::domain;

struct TestEntity: public entity::Entity<entity::IdType>
{
    int counter = 0;

    TestEntity(entity::IdType id):
        entity::Entity<entity::IdType>(id)
    {}
};

TEST_CASE( "Get entities count", "[repository]" )
{
    auto repository = dory::domain::EntityRepository<TestEntity>(
        std::initializer_list<TestEntity>
        {
            TestEntity(1),
            TestEntity(2)
        });

    REQUIRE(repository.count() == 2);
}

TEST_CASE( "Iterator", "[repository]" )
{
    auto repository = dory::domain::EntityRepository<TestEntity>(
        std::initializer_list<TestEntity>
        {
            TestEntity(1),
            TestEntity(2)
        });

    std::vector<entity::IdType> ids;

    repository.forEach([&ids](const TestEntity& entity)
    {
        ids.push_back(entity.id);
    });

    REQUIRE(ids.size() == 2);
    REQUIRE(ids[0] == 1);
    REQUIRE(ids[1] == 2);
}

TEST_CASE( "Store entity", "[repository]" )
{
    auto repository = dory::domain::EntityRepository<TestEntity>();

    auto entity = repository.store(TestEntity(1));

    REQUIRE(entity.id == 1);
    REQUIRE(repository.count() == 1);
}

TEST_CASE( "Remove", "[repository]" )
{
    auto repository = dory::domain::EntityRepository<TestEntity>(
        std::initializer_list<TestEntity>
        {
            TestEntity(1),
            TestEntity(2)
        });

    repository.remove(1);

    std::vector<entity::IdType> ids;

    repository.forEach([&ids](const TestEntity& entity)
    {
        ids.push_back(entity.id);
    });

    REQUIRE(ids.size() == 1);
    REQUIRE(ids[0] == 2);
}

TEST_CASE( "create/count/get/update/remove entity", "[static poly repository]" )
{
    auto repository = dory::domain::EntityRepository<TestEntity>{};

    repository.store(TestEntity(1));

    REQUIRE(repository.count() == 1);

    {
        auto entityOptional = repository.get(1);
        REQUIRE(entityOptional.has_value());
        auto& entity = entityOptional.value();
        REQUIRE(entity.id == 1);
        REQUIRE(entity.counter == 0);

        entity.counter = 1;
        repository.store(entity);
        REQUIRE(repository.count() == 1);
    }

    {
        auto entityOptional = repository.get(1);
        REQUIRE(entityOptional.has_value());
        auto& entity = entityOptional.value();
        REQUIRE(entity.id == 1);
        REQUIRE(entity.counter == 1);

        entity.id = 2;
        entity.counter = 0;
        repository.store(entity);
        REQUIRE(repository.count() == 2);
    }

    {
        repository.remove(2);

        REQUIRE(repository.count() == 1);

        auto entityOptional = repository.get(2);
        REQUIRE(!entityOptional.has_value());
    }
}

TEST_CASE( "forEach", "[static poly repository]" )
{
    auto repository = dory::domain::EntityRepository<TestEntity>(
        {
            TestEntity(1),
            TestEntity(2)
        });

    REQUIRE(repository.count() == 2);

    std::size_t count = 0;
    entity::IdType ids[2];

    repository.forEach([&](const TestEntity& entity)
    {
        REQUIRE(count < 2);
        ids[count++] = entity.id;
    });

    REQUIRE(count == 2);
    REQUIRE(ids[0] == 1);
    REQUIRE(ids[1] == 2);
}
