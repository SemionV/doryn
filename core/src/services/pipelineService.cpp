#include <dory/core/registry.h>
#include <dory/core/services/pipelineService.h>
#include <dory/core/events/eventTypes.h>
#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/controllers/iController.h>

namespace dory::core::services
{
    PipelineService::PipelineService(Registry& registry):
            _registry(registry)
    {}

    void PipelineService::update(resources::DataContext& context, const generic::model::TimeSpan& timeStep)
    {
        _registry.get<repositories::IPipelineRepository>([&context, &timeStep](repositories::IPipelineRepository* repository){
            auto nodes = repository->getPipelineNodes();

            for(auto& node : nodes)
            {
                if(node.attachedController)
                {
                    auto controllerRef = node.attachedController->lock();
                    if(controllerRef)
                    {
                        auto controller = std::static_pointer_cast<controllers::IController>(*controllerRef);
                        if(controller)
                        {
                            controller->update(node.id, timeStep, context);
                        }
                    }
                }
                else if(node.updateFunction)
                {
                    auto updateFunctionRef = node.updateFunction->lock();
                    if(updateFunctionRef)
                    {
                        (*updateFunctionRef)(node.id, timeStep, context);
                    }
                }
            }
        });
    }

    void PipelineService::initialize(resources::DataContext& context)
    {
        _registry.get<events::pipeline::Bundle::IDispatcher>([&context](events::pipeline::Bundle::IDispatcher* dispatcher){
            dispatcher->fire(context, events::pipeline::Initialize{});
        });
    }

    void PipelineService::stop(resources::DataContext& context)
    {
        _registry.get<repositories::IPipelineRepository>([&context](repositories::IPipelineRepository* repository){
            auto nodes = repository->getPipelineNodes();

            for(auto& node : nodes)
            {
                if(node.attachedController)
                {
                    auto controllerRef = node.attachedController->lock();
                    if(controllerRef)
                    {
                        auto controller = std::static_pointer_cast<controllers::IController>(*controllerRef);
                        if(controller)
                        {
                            controller->stop(node.id, context);
                        }
                    }
                }
            }
        });
    }
}