#include "baseTests/dependencies.h"

using namespace dory::domain;

TEST_CASE( "Load Pipeline", "[pipelineService]" )
{
    auto groupsRepository = std::make_shared<dory::EntityRepository<entity::PipelineGroup>>(
        std::initializer_list<entity::PipelineGroup>{
            entity::PipelineGroup(2, "group 2", 1),
            entity::PipelineGroup(1, "group 1", 0)
        });
    auto groupsReader = std::make_shared<dory::RepositoryReader<entity::PipelineGroup>>(groupsRepository);

    auto nodesRepository = std::make_shared<dory::EntityRepository<entity::PipelineNode>>(
        std::initializer_list<entity::PipelineNode>{
            entity::PipelineNode(2, 1, nullptr, 0, 1),
            entity::PipelineNode(1, 1, nullptr, 0, 0)
        });
    auto nodesReader = std::make_shared<dory::RepositoryReader<entity::PipelineNode>>(nodesRepository);

    auto pipelineService = std::make_shared<services::PipelineService>(groupsReader, nodesReader);
    auto pipeline = pipelineService->getPipeline();

    REQUIRE(pipeline->groups.size() == 2);
    auto group = *(pipeline->groups.begin());
    REQUIRE(group->groupEntity);
    REQUIRE(group->groupEntity->id == 1);
    REQUIRE(group->groupEntity->name == "group 1");
    REQUIRE(group->nodeEntities.size() == 2);
    auto i = group->nodeEntities.begin();
    auto node = *i;
    REQUIRE(node->id == 1);
    REQUIRE(node->groupId == 1);
    REQUIRE(node->priority == 0);
    ++i;
    node = *i;
    REQUIRE(node->id == 2);
    REQUIRE(node->groupId == 1);
    REQUIRE(node->priority == 1);
}