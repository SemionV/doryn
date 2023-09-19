#include "baseTests/dependencies.h"

using namespace fakeit;
using namespace dory::domain;

template<class TEntity>
class TestTraverseIterator: public dory::ITraverseIterator<TEntity>
{
    private:
        std::vector<TEntity> entities;
        typename std::vector<TEntity>::iterator current;

    public:
        TestTraverseIterator(std::initializer_list<TEntity> list):
            entities(list),
            current(entities.begin())
        {}

        TestTraverseIterator(std::vector<TEntity> list):
            entities(list),
            current(entities.begin())
        {}

        TEntity* next() override
        {
            if(current != entities.end())
            {
                auto result = &(*current);
                current++;

                return result;
            }

            return nullptr;
        }
};

TEST_CASE( "Load Pipeline", "[pipelineService]" )
{
    /*fakeit::Mock<dory::RepositoryReader<entity::PipelineGroup>> groupsReaderMock;
    fakeit::Mock<dory::RepositoryReader<entity::PipelineNode>> nodesReaderMock;

    auto groupsIterator = std::make_shared<TestTraverseIterator<entity::PipelineGroup>>(
        std::vector<entity::PipelineGroup>
        {
            {"group 1", 0}
        }
    );

    When(Method(groupsReaderMock, getTraverseIterator)).Return(
        std::unique_ptr<dory::ITraverseIterator<entity::PipelineGroup>>(groupsIterator.get())
    ); 

    std::shared_ptr<dory::RepositoryReader<entity::PipelineGroup>> groupsReader(&groupsReaderMock.get(), [](auto) {});
    std::shared_ptr<dory::RepositoryReader<entity::PipelineNode>> nodesReader(&nodesReaderMock.get(), [](auto) {});

    auto pipelineService = std::make_shared<services::PipelineService>(groupsReader, nodesReader);

    pipelineService->getPipeline();*/
}