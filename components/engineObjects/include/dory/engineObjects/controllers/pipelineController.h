#pragma once

#include <dory/engine/controllers/pipelineController.h>
#include <dory/engine/events/eventTypes.h>
#include <dory/engine/controllers/controller.h>
#include <dory/engine/repositories/pipelineRepository.h>

namespace dory::engine::controllers
{
    template<typename TPipelineRepository>
    class PipelineController: public IPipelineController<PipelineController<TPipelineRepository>>
    {
    private:
        events::mainController::Dispatcher<DataContextType>& _mainControllerEventHub;
        repositories::IPipelineRepository<TPipelineRepository, DataContextType>& _pipelineRepository;

    public:
        explicit PipelineController(events::mainController::Dispatcher<DataContextType>& mainControllerEventHub,
                                repositories::IPipelineRepository<TPipelineRepository, DataContextType>& pipelineRepository);

        void updateImpl(DataContextType& context, const resources::TimeSpan& timeStep);
        void initializeImpl(DataContextType& context);
        void stopImpl(DataContextType& context);

    private:
        template<typename F, typename... Ts>
        static void touchPipelineNodes(std::list<std::shared_ptr<resources::object::PipelineNode<DataContextType>>> pipelineNodes, DataContextType& context, F functor, Ts... arguments)
        {
            auto end = pipelineNodes.end();

            for(auto i = pipelineNodes.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }

        template<typename F, typename... Ts>
        static void touchNode(std::shared_ptr<resources::object::PipelineNode<DataContextType>> node, DataContextType& context, F functor, Ts... arguments)
        {
            functor(node, context, arguments...);

            auto end = node->children.end();

            for(auto i = node->children.begin(); i != end; ++i)
            {
                touchNode(*i, context, functor, arguments...);
            }
        }
    };
}