#pragma once

#include <string>
#include <utility>

namespace dory::entity
{
    using IdType = unsigned int;
    constexpr static const IdType nullId = 0;

    template<typename T = IdType>
    struct Entity
    {
        T id;
        explicit Entity(T id):
                id(id)
        {}
    };

    template<typename TController>
    struct PipelineNode: Entity<IdType>
    {
        TController* attachedController;
        IdType parentNodeId;
        std::string name;
        int priority;

        explicit PipelineNode(IdType id, TController* attachedController = nullptr, int priority = 0, IdType parentNodeId = nullId, std::string name = ""):
                Entity(id),
                attachedController(attachedController),
                parentNodeId(parentNodeId),
                priority(priority),
                name(std::move(name))
        {}
    };
}