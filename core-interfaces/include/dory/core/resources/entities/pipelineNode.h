#pragma once

#include "../entity.h"
#include <dory/generic/model.h>
#include <functional>
#include <dory/generic/extension/resourceHandle.h>
#include <dory/core/resources/name.h>
#include <string>
#include <optional>
#include <utility>

namespace dory::core
{
    class ITrigger;
    class IController;
}

namespace dory::core::resources::entities
{
    struct NodeUpdateCounter
    {
        std::size_t count;
        generic::model::TimeSpan deltaTime;
    };

    struct PipelineNode: public Entity<>
    {
        using UpdateFunctionType = std::function<void(IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context)>;
        using UpdateTriggerType = std::function<NodeUpdateCounter(IdType nodeId, const generic::model::TimeSpan& timeStep, DataContext& context)>;
        using ControllerPointerType = std::shared_ptr<IController>;
        using TriggerPointerType = std::shared_ptr<ITrigger>;

        std::optional<generic::extension::ResourceHandle<ControllerPointerType>> attachedController {};
        std::optional<generic::extension::ResourceHandle<UpdateFunctionType>> updateFunction {};
        std::optional<generic::extension::ResourceHandle<TriggerPointerType>> trigger {};
        std::optional<generic::extension::ResourceHandle<UpdateTriggerType>> triggerFunction {};
        IdType parentNodeId {};
        Name name {};
        bool skipUpdate {};
        IdType sceneId {};

        PipelineNode() = default;

        explicit PipelineNode(const IdType id,
                              const IdType parentNodeId = nullId,
                              Name name = {}):
                Entity(id),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(const IdType id,
                              generic::extension::ResourceHandle<UpdateTriggerType> triggerFunction,
                              const IdType parentNodeId = nullId,
                              Name name = {}):
                Entity(id),
                triggerFunction(triggerFunction),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<ControllerPointerType> attachedController,
                              const IdType parentNodeId = nullId,
                              Name name = {}):
                attachedController(std::move(attachedController)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<ControllerPointerType> attachedController,
                              generic::extension::ResourceHandle<UpdateTriggerType> triggerFunction,
                              const IdType parentNodeId = nullId,
                              Name name = {}):
                attachedController(std::move(attachedController)),
                triggerFunction(triggerFunction),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<UpdateFunctionType> updateFunction,
                              const IdType parentNodeId = nullId,
                              Name name = {}):
                updateFunction(std::move(updateFunction)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<UpdateFunctionType> updateFunction,
                              generic::extension::ResourceHandle<UpdateTriggerType> triggerFunction,
                              const IdType parentNodeId = nullId,
                              Name name = {}):
                updateFunction(std::move(updateFunction)),
                triggerFunction(triggerFunction),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}
    };
}
