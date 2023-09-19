#pragma once

#include "base/dependencies.h"

namespace dory::domain::entity
{
    using IdType = unsigned int;
    const IdType nullId = 0;

    template<typename TId>
    struct Entity
    {
        TId id;
    };

    struct Camera: public Entity<IdType>
    {
    };

    struct Window: public Entity<IdType>
    {
    };

    struct Viewport
    {
        int x;
        int y;
        int width;
        int height;

        Viewport(int x, int y, int width, int height):
            x(x),
            y(y),
            width(width),
            height(height)
        {            
        }
    };

    struct View: public Entity<IdType>
    {
        IdType windowId;
        IdType cameraId;
        Viewport viewport;

        View(IdType windowId, IdType cameraId, Viewport viewport):
            windowId(windowId),
            cameraId(cameraId),
            viewport(viewport)
        {}
    };

    struct PipelineNode: Entity<IdType>
    {
        std::shared_ptr<void> attachedController;
        IdType attachedGroupId;
        IdType groupId;
        int priority;

        PipelineNode(){};

        PipelineNode(IdType groupId, std::shared_ptr<void> attachedController = nullptr, IdType attachedGroupId = 0, int priority = 0):
            attachedController(attachedController),
            groupId(groupId),
            attachedGroupId(attachedGroupId),
            priority(priority)
        {}
    };

    struct PipelineGroup: public Entity<IdType>
    {
        std::string name;
        int priority;

        PipelineGroup(){};

        PipelineGroup(std::string name, int priority = 0):
            name(name),
            priority(priority)
        {}
    };
}