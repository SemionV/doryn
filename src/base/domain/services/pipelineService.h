#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"
#include "base/doryExport.h"
#include "base/domain/entity.h"
#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename T>
    bool compareNodes(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    template<typename TServiceLocator>
    class PipelineService: Service<TServiceLocator>
    {
    public:
        explicit PipelineService(TServiceLocator& serviceLocator):
                Service<TServiceLocator>(serviceLocator)
        {}

        std::list<std::shared_ptr<object::PipelineNode>> getPipeline()
        {
            std::list<std::shared_ptr<object::PipelineNode>> nodes;

            this->services.pipelineNodeRepository.forEach([this, &nodes](const entity::PipelineNode& nodeEntity)
            {
                if(nodeEntity.parentNodeId == entity::nullId)
                {
                    nodes.emplace_back(loadNode(nodeEntity));
                }
            });

            nodes.sort(compareNodes<object::PipelineNode>);

            return nodes;
        }

    private:
        std::shared_ptr<object::PipelineNode> loadNode(const entity::PipelineNode& nodeEntity)
        {
            auto node = std::make_shared<object::PipelineNode>(nodeEntity);

            this->services.pipelineNodeRepository.forEach([this, &node](const auto& nodeEntity)
            {
                if(nodeEntity.parentNodeId == node->nodeEntity.id)
                {
                    node->children.emplace_back(loadNode(nodeEntity));
                }
            });

            node->children.sort(compareNodes<object::PipelineNode>);

            return node;
        }
    };

    template<typename TImplementation>
    class IPipelineRepository: public IEntityRepository<TImplementation, entity::PipelineNode, entity::IdType>
    {
    public:
        std::list<std::shared_ptr<object::PipelineNode>> getPipeline()
        {
            return this->toImplementation()->getPipelineImpl();
        }
    };

    template<typename TEntity, typename TIdType>
    class PipelineRepository:
            public IPipelineRepository<PipelineRepository<TEntity, TIdType>>,
            public EntityRepository2<TEntity, TIdType>
    {
    private:
        using InterfaceType = IPipelineRepository<PipelineRepository<TEntity, TIdType>>;

    public:
        std::list<std::shared_ptr<object::PipelineNode>> getPipelineImpl()
        {
            std::list<std::shared_ptr<object::PipelineNode>> nodes;

            InterfaceType::forEach([this, &nodes](const TEntity& nodeEntity)
                                           {
                                               if(nodeEntity.parentNodeId == entity::nullId)
                                               {
                                                   nodes.emplace_back(loadNode(nodeEntity));
                                               }
                                           });

            nodes.sort(compareNodes<object::PipelineNode>);

            return nodes;
        }

    private:
        std::shared_ptr<object::PipelineNode> loadNode(const TEntity& nodeEntity)
        {
            auto node = std::make_shared<object::PipelineNode>(nodeEntity);

            InterfaceType::forEach([this, &node](const auto& nodeEntity)
                                           {
                                               if(nodeEntity.parentNodeId == node->nodeEntity.id)
                                               {
                                                   node->children.emplace_back(loadNode(nodeEntity));
                                               }
                                           });

            node->children.sort(compareNodes<object::PipelineNode>);

            return node;
        }
    };

    template<typename TImplementation>
    class IPipelineManager: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void configurePipeline()
        {
            return this->toImplementation()->configurePipelineImpl();
        }
    };

    template<typename TDataContext, typename TPipelineNodeRepository>
    class PipelineManager: IPipelineManager<PipelineManager<TDataContext, TPipelineNodeRepository>>
    {
    private:
        using PipelineNodeRepository = IEntityRepository<TPipelineNodeRepository, entity::PipelineNode, entity::IdType>;
        PipelineNodeRepository& pipelineRepository;

    public:
        explicit PipelineManager(PipelineNodeRepository& pipelineRepository):
                pipelineRepository(pipelineRepository)
        {}

        void configurePipelineImpl()
        {
            auto inputGroupNode = dory::domain::entity::PipelineNode(entity::nullId,
                                                                     nullptr,
                                                                     dory::domain::entity::PipelineNodePriority::Default,
                                                                     entity::nullId,
                                                                     "input group");
        }
    };
}