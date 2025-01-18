#pragma once

#include <dory/core/repositories/iPipelineRepository.h>
#include <dory/core/repository.h>

namespace dory::core::repositories
{
    class PipelineRepository: public IPipelineRepository
    {
    public:
        using NodeListType = std::vector<EntityType>;

    private:
        NodeListType _nodes;
        IdType _counter { 1 };

        IdType insertNode(const EntityType& node, const NodeListType::iterator& after);
        IdType insertNode(const EntityType& node);

    public:
        PipelineRepository() = default;

        PipelineRepository(std::initializer_list<EntityType>&& entities):
                _nodes(entities),
                _counter(entities.size() + 1)
        {}

        std::span<EntityType> getPipelineNodes() override;
        IdType addNode(const EntityType& pipelineNode) override;
        IdType addTriggerNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateTriggerType& updateTrigger) override;
        IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, EntityType::ControllerPointerType controller) override;
        IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, EntityType::ControllerPointerType controller, const EntityType::UpdateTriggerType& updateTrigger) override;
        IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction) override;
        IdType addNode(IdType parentNode, const generic::extension::LibraryHandle& libraryHandle, const EntityType::UpdateFunctionType& updateFunction, const EntityType::UpdateTriggerType& updateTrigger) override;
        void removeNode(IdType id) override;
    };
}