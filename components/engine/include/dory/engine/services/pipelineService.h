#pragma once

#include "dory/engine/resources/object.h"
#include "dory/engine/repositories/entityRepository.h"
#include "dory/engine/resources/entity.h"
#include "dory/generics/typeTraits.h"
#include "dory/engine/repositories/pipelineRepository.h"

namespace dory::engine::services
{
    template<typename TImplementation, typename TDataContext>
    class IPipelineService: NonCopyable, public StaticInterface<TImplementation>
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
    class PipelineService: public IPipelineService<PipelineService<TDataContext, TWindowControllerFactory, TPipelineRepository>, TDataContext>
    {
    private:
        using PipelineNodeType = resources::entity::PipelineNode<TDataContext>;

        using WindowControllerFactoryType = dory::IServiceFactory<TWindowControllerFactory>;
        WindowControllerFactoryType& windowControllerFactory;

        using PipelineRepositoryType = repositories::IPipelineRepository<TPipelineRepository, TDataContext>;
        PipelineRepositoryType& pipelineRepository;

    public:
        explicit PipelineService(WindowControllerFactoryType& windowControllerFactory,
                                 PipelineRepositoryType& pipelineRepository):
                windowControllerFactory(windowControllerFactory),
                pipelineRepository(pipelineRepository)
        {}

        void configurePipelineImpl(TDataContext&  context)
        {
            auto inputGroupNode = pipelineRepository.store(PipelineNodeType(resources::entity::nullId,
                                                                                nullptr,
                                                                                resources::entity::PipelineNodePriority::Default,
                                                                                resources::entity::nullId,
                                                                                "input group"));

            auto outputGroupNode = pipelineRepository.store(PipelineNodeType(resources::entity::nullId,
                                                                                nullptr,
                                                                                resources::entity::PipelineNodePriority::First,
                                                                                resources::entity::nullId,
                                                                                "output group"));

            context.inputGroupNodeId = inputGroupNode.id;
            context.outputGroupNodeId = outputGroupNode.id;

            auto windowController = windowControllerFactory.createInstance();
            auto windowControllerNode = pipelineRepository.store(PipelineNodeType(resources::entity::nullId,
                                                                                  windowController,
                                                                                  resources::entity::PipelineNodePriority::Default,
                                                                                  inputGroupNode.id));
            windowController->initialize(windowControllerNode.id, context);
        }
    };
}