#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <dory/core/repositories/pipelineRepository.h>
#include <dory/core/resources/entity.h>

TEST(PipelineRepository, addNode)
{
    auto pipelineRepository = dory::core::repositories::PipelineRepository{};

    auto node = dory::core::resources::entities::PipelineNode{};

    auto id = pipelineRepository.addNode(node);
    EXPECT_FALSE(id == dory::core::repositories::PipelineRepository::IdType{});

    auto nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 1);
    EXPECT_EQ(id, nodes[0].id);
}

TEST(PipelineRepository, addNodes)
{
    auto pipelineRepository = dory::core::repositories::PipelineRepository{
        {
                dory::core::resources::entities::PipelineNode{ 1 },
                dory::core::resources::entities::PipelineNode{ 2, 1 },
                dory::core::resources::entities::PipelineNode{ 4, 2 },
                dory::core::resources::entities::PipelineNode{ 5, 2 },
                dory::core::resources::entities::PipelineNode{ 3, 1 },
                dory::core::resources::entities::PipelineNode{ 6, 3 },
                dory::core::resources::entities::PipelineNode{ 7 },
                dory::core::resources::entities::PipelineNode{ 8, 7 },
                dory::core::resources::entities::PipelineNode{ 10, 8 },
                dory::core::resources::entities::PipelineNode{ 11, 8 },
                dory::core::resources::entities::PipelineNode{ 9, 7 },
                dory::core::resources::entities::PipelineNode{ 12, 9 },
                dory::core::resources::entities::PipelineNode{ 13, 9 },
        }
    };

    auto node = dory::core::resources::entities::PipelineNode{{}, 1};
    auto id = pipelineRepository.addNode(node);
    EXPECT_FALSE(id == dory::core::repositories::PipelineRepository::IdType{});
    auto nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 14);
    EXPECT_EQ(nodes[6].id, id);

    node = dory::core::resources::entities::PipelineNode{{}, 8};
    id = pipelineRepository.addNode(node);
    EXPECT_FALSE(id == dory::core::repositories::PipelineRepository::IdType{});
    nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 15);
    EXPECT_EQ(nodes[11].id, id);

    node = dory::core::resources::entities::PipelineNode{{}, 9};
    id = pipelineRepository.addNode(node);
    EXPECT_FALSE(id == dory::core::repositories::PipelineRepository::IdType{});
    nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 16);
    EXPECT_EQ(nodes[15].id, id);
}

TEST(PipelineRepository, removeNode)
{
    auto pipelineRepository = dory::core::repositories::PipelineRepository{
            {
                    dory::core::resources::entities::PipelineNode{ 1 }
            }
    };

    pipelineRepository.removeNode(1);

    auto nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 0);
}

TEST(PipelineRepository, removeNodes)
{
    auto pipelineRepository = dory::core::repositories::PipelineRepository{
            {
                    dory::core::resources::entities::PipelineNode{ 1 },
                    dory::core::resources::entities::PipelineNode{ 2, 1 },
                    dory::core::resources::entities::PipelineNode{ 4, 2 },
                    dory::core::resources::entities::PipelineNode{ 5, 2 },
                    dory::core::resources::entities::PipelineNode{ 3, 1 },
                    dory::core::resources::entities::PipelineNode{ 6, 3 },
                    dory::core::resources::entities::PipelineNode{ 7 },
                    dory::core::resources::entities::PipelineNode{ 8, 7 },
                    dory::core::resources::entities::PipelineNode{ 10, 8 },
                    dory::core::resources::entities::PipelineNode{ 11, 8 },
                    dory::core::resources::entities::PipelineNode{ 9, 7 },
                    dory::core::resources::entities::PipelineNode{ 12, 9 },
                    dory::core::resources::entities::PipelineNode{ 13, 9 },
            }
    };

    pipelineRepository.removeNode(9);
    auto nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 10);
    EXPECT_EQ(nodes[9].id, 11);

    pipelineRepository.removeNode(1);
    nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 4);
    EXPECT_EQ(nodes[0].id, 7);
    EXPECT_EQ(nodes[3].id, 11);

    pipelineRepository.removeNode(10);
    nodes = pipelineRepository.getPipelineNodes();
    EXPECT_EQ(std::distance(nodes.begin(),nodes.end()), 3);
    EXPECT_EQ(nodes[0].id, 7);
    EXPECT_EQ(nodes[2].id, 11);
}