#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"
#include "base/typeComponents.h"

namespace dory::domain::services
{
    template<typename T>
    bool compareNodes(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b)
    {
        return a->nodeEntity.priority < b->nodeEntity.priority;
    }

    template<typename TImplementation, typename TDataContext>
    class IPipelineRepository: public IEntityRepository<TImplementation, entity::PipelineNode<TDataContext>, entity::IdType>
    {
    public:
        std::list<std::shared_ptr<object::PipelineNode<TDataContext>>> getPipeline()
        {
            return this->toImplementation()->getPipelineImpl();
        }
    };

    template<typename TDataContext, typename TEntity, typename TIdType = entity::IdType>
    class PipelineRepository:
            public IPipelineRepository<PipelineRepository<TDataContext, TEntity, TIdType>, TDataContext>,
            public EntityRepository<TEntity, TIdType>
    {
    private:
        using InterfaceType = IPipelineRepository<PipelineRepository<TDataContext, TEntity, TIdType>, TDataContext>;

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<TEntity>&& entities):
                EntityRepository<TEntity, TIdType>(std::move(entities))
        {}

        std::list<std::shared_ptr<object::PipelineNode<TDataContext>>> getPipelineImpl()
        {
            std::list<std::shared_ptr<object::PipelineNode<TDataContext>>> nodes;

            InterfaceType::forEach([this, &nodes](const TEntity& nodeEntity)
                                           {
                                               if(nodeEntity.parentNodeId == entity::nullId)
                                               {
                                                   nodes.emplace_back(loadNode(nodeEntity));
                                               }
                                           });

            nodes.sort(compareNodes<object::PipelineNode<TDataContext>>);

            return nodes;
        }

    private:
        std::shared_ptr<object::PipelineNode<TDataContext>> loadNode(const TEntity& nodeEntity)
        {
            auto node = std::make_shared<object::PipelineNode<TDataContext>>(nodeEntity);

            InterfaceType::forEach([this, &node](const auto& nodeEntity)
                                           {
                                               if(nodeEntity.parentNodeId == node->nodeEntity.id)
                                               {
                                                   node->children.emplace_back(loadNode(nodeEntity));
                                               }
                                           });

            node->children.sort(compareNodes<object::PipelineNode<TDataContext>>);

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
        using PipelineNodeType = entity::PipelineNode<TDataContext>;

        using ConsoleControllerFactoryType = dory::IServiceFactory<TConsoleControllerFactory>;
        ConsoleControllerFactoryType& consoleControllerFactory;

        using WindowControllerFactoryType = dory::IServiceFactory<TWindowControllerFactory>;
        WindowControllerFactoryType& windowControllerFactory;

        using PipelineRepositoryType = services::IPipelineRepository<TPipelineRepository, TDataContext>;
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
            auto inputGroupNode = pipelineRepository.store(PipelineNodeType(entity::nullId,
                                                                                nullptr,
                                                                                entity::PipelineNodePriority::Default,
                                                                                entity::nullId,
                                                                                "input group"));

            auto outputGroupNode = pipelineRepository.store(PipelineNodeType(entity::nullId,
                                                                                 nullptr,
                                                                                 entity::PipelineNodePriority::First,
                                                                                 entity::nullId,
                                                                                 "output group"));

            context.inputGroupNodeId = inputGroupNode.id;
            context.outputGroupNodeId = outputGroupNode.id;

            auto consoleController = consoleControllerFactory.createInstance();
            auto consoleControllerNode = pipelineRepository.store(PipelineNodeType(entity::nullId,
                                                                                       consoleController,
                                                                                       entity::PipelineNodePriority::Default,
                                                                                       inputGroupNode.id));
            consoleController->initialize(consoleControllerNode.id, context);

            auto windowController = windowControllerFactory.createInstance();
            auto windowControllerNode = pipelineRepository.store(PipelineNodeType(entity::nullId,
                                                                                      windowController,
                                                                                      entity::PipelineNodePriority::Default,
                                                                                      inputGroupNode.id));
            windowController->initialize(windowControllerNode.id, context);
        }
    };
}