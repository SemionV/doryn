#pragma once

#include <dory/core/iController.h>
#include <dory/core/resources/entity.h>
#include <dory/core/resources/entities/pipelineNode.h>
#include <dory/generic/baseTypes.h>
#include <dory/core/resources/name.h>

namespace dory::core::repositories
{
    class IPipelineRepository: public generic::Interface
    {
    public:
        using EntityType = resources::entities::PipelineNode;
        using IdType = EntityType::IdType;

        virtual std::span<EntityType> getPipelineNodes() = 0;
        virtual IdType addNode(const EntityType& pipelineNode) = 0;
        virtual IdType addTriggerNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateTriggerType& updateTrigger) = 0;
        virtual IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, EntityType::ControllerPointerType controller) = 0;
        virtual IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, EntityType::ControllerPointerType controller, const EntityType::UpdateTriggerType& updateTrigger) = 0;
        virtual IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction) = 0;
        virtual IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction, const EntityType::UpdateTriggerType& updateTrigger) = 0;
        virtual void removeNode(IdType id) = 0;
        virtual EntityType* getNode(const resources::Name& name) = 0;
    };
}