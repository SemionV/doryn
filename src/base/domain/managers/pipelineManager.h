#pragma once

#include "base/domain/object.h"
#include "base/domain/entityRepository.h"
#include "base/domain/entity.h"
#include "base/typeComponents.h"
#include "base/domain/repositories/pipelineRepository.h"

namespace dory::domain::managers
{
    template<typename TImplementation, typename TDataContext>
    class IPipelineManager: NonCopyable, public StaticInterface<TImplementation>
    {
    public:
        void configurePipeline(TDataContext&  context)
        {
            this->toImplementation()->configurePipelineImpl(context);
        }
    };

    template<typename TDataContext,
            typename TWindowControllerFactory,
            typename TPipelineRepository>
    class PipelineManager: public IPipelineManager<PipelineManager<TDataContext, TWindowControllerFactory, TPipelineRepository>, TDataContext>
    {
    private:
        using PipelineNodeType = entity::PipelineNode<TDataContext>;

        using WindowControllerFactoryType = dory::IServiceFactory<TWindowControllerFactory>;
        WindowControllerFactoryType& windowControllerFactory;

        using PipelineRepositoryType = repositories::IPipelineRepository<TPipelineRepository, TDataContext>;
        PipelineRepositoryType& pipelineRepository;

    public:
        explicit PipelineManager(WindowControllerFactoryType& windowControllerFactory,
                                 PipelineRepositoryType& pipelineRepository):
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

            auto windowController = windowControllerFactory.createInstance();
            auto windowControllerNode = pipelineRepository.store(PipelineNodeType(entity::nullId,
                                                                                      windowController,
                                                                                      entity::PipelineNodePriority::Default,
                                                                                      inputGroupNode.id));
            windowController->initialize(windowControllerNode.id, context);
        }
    };
}