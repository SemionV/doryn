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
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<TEntity>&& entities):
                EntityRepository2<TEntity, TIdType>(std::move(entities))
        {}

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

    template<typename TImplementation, typename TDataContext>
    class IPipelineManager: Uncopyable, public StaticInterface<TImplementation>
    {
    public:
        void configurePipeline(TDataContext&  context)
        {
            this->toImplementation()->configurePipelineImpl(context);
        }
    };

    template<typename TDataContext,
            typename TConsoleControllerFactory,
            typename TWindowControllerFactory,
            typename TPipelineRepository>
    class PipelineManager: public IPipelineManager<PipelineManager<TDataContext,
            TConsoleControllerFactory, TWindowControllerFactory, TPipelineRepository>, TDataContext>
    {
    private:
        using ConsoleControllerFactoryType = dory::IServiceFactory<TConsoleControllerFactory>;
        ConsoleControllerFactoryType& consoleControllerFactory;

        using WindowControllerFactoryType = dory::IServiceFactory<TWindowControllerFactory>;
        WindowControllerFactoryType& windowControllerFactory;

        using PipelineRepositoryType = services::IPipelineRepository<TPipelineRepository>;
        PipelineRepositoryType& pipelineRepository;

    public:
        explicit PipelineManager(ConsoleControllerFactoryType& consoleControllerFactory,
                                 WindowControllerFactoryType& windowControllerFactory,
                                 PipelineRepositoryType& pipelineRepository):
                consoleControllerFactory(consoleControllerFactory),
                windowControllerFactory(windowControllerFactory),
                pipelineRepository(pipelineRepository)
        {}

        void configurePipelineImpl(TDataContext&  context)
        {
            auto inputGroupNode = pipelineRepository.store(entity::PipelineNode(entity::nullId,
                                                                                nullptr,
                                                                                entity::PipelineNodePriority::Default,
                                                                                entity::nullId,
                                                                                "input group"));

            auto outputGroupNode = pipelineRepository.store(entity::PipelineNode(entity::nullId,
                                                                                 nullptr,
                                                                                 entity::PipelineNodePriority::First,
                                                                                 entity::nullId,
                                                                                 "output group"));

            context.inputGroupNodeId = inputGroupNode.id;
            context.outputGroupNodeId = outputGroupNode.id;

            auto consoleController = consoleControllerFactory.createInstance();
            auto consoleControllerNode = pipelineRepository.store(entity::PipelineNode(entity::nullId,
                                                                                       consoleController,
                                                                                       entity::PipelineNodePriority::Default,
                                                                                       inputGroupNode.id));
            consoleController->initialize(consoleControllerNode.id, context);

            auto windowController = windowControllerFactory.createInstance();
            auto windowControllerNode = pipelineRepository.store(entity::PipelineNode(entity::nullId,
                                                                                      windowController,
                                                                                      entity::PipelineNodePriority::Default,
                                                                                      inputGroupNode.id));
            windowController->initialize(windowControllerNode.id, context);
        }
    };
}