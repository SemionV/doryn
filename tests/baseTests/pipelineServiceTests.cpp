#include "dependencies.h"

using namespace dory::domain;

TEST_CASE( "Load Pipeline", "[pipelineRepository]" )
{
    using PipelineNodeType = entity::PipelineNode<int>;

    auto pipelineService = repositories::PipelineRepository<int, PipelineNodeType, entity::IdType>(
            std::initializer_list<PipelineNodeType>{
                    PipelineNodeType(2, nullptr, entity::PipelineNodePriority::First, entity::nullId),
                    PipelineNodeType(1, nullptr, entity::PipelineNodePriority::Default, entity::nullId),
                    PipelineNodeType(3, nullptr, entity::PipelineNodePriority::First, 1),
                    PipelineNodeType(4, nullptr, entity::PipelineNodePriority::Default, 1),
                    PipelineNodeType(5, nullptr, entity::PipelineNodePriority::Default, 4),
                    PipelineNodeType(6, nullptr, entity::PipelineNodePriority::Default, 2),
            });
    auto pipeline = pipelineService.getPipeline();

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