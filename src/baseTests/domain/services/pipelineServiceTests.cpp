#include "baseTests/dependencies.h"
#include "base/base.h"

using namespace dory::domain;

TEST_CASE( "Load Pipeline", "[pipelineService]" )
{
    auto nodesRepository = std::make_shared<dory::domain::EntityRepository<entity::PipelineNode>>(
        std::initializer_list<entity::PipelineNode>{
            entity::PipelineNode(2, nullptr, 1, dory::entity::nullId),
            entity::PipelineNode(1, nullptr, 0, dory::entity::nullId),
            entity::PipelineNode(3, nullptr, 1, 1),
            entity::PipelineNode(4, nullptr, 0, 1),
            entity::PipelineNode(5, nullptr, 0, 4),
            entity::PipelineNode(6, nullptr, 0, 2),
        });
    auto nodesReader = std::make_shared<dory::domain::RepositoryReader<entity::PipelineNode>>(nodesRepository);

    auto pipelineService = std::make_shared<services::PipelineService>(nodesReader);
    auto pipeline = pipelineService->getPipeline();

    REQUIRE(pipeline.size() == 2);
    
    auto i = pipeline.begin();
    REQUIRE((*i)->nodeEntity.id == 1);
    auto children = (*i)->children;
    REQUIRE(children.size() == 2);
    auto j = children.begin();
    REQUIRE((*j)->nodeEntity.id == 4);
    REQUIRE((*j)->children.size() == 1);
    j++;
    REQUIRE((*j)->nodeEntity.id == 3);
    
    ++i;
    REQUIRE((*i)->nodeEntity.id == 2);
    children = (*i)->children;
    REQUIRE(children.size() == 1);
    j = children.begin();
    REQUIRE((*j)->nodeEntity.id == 6);
}