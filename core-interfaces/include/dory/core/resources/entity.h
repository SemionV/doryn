#pragma once

#include <memory>
#include <string>
#include <functional>
#include <utility>
#include <type_traits>
#include <dory/generic/extension/resourceHandle.h>
#include <dory/generic/model.h>
#include "id.h"
#include "dataContext.h"
#include "windowSystem.h"
#include "graphicalSystem.h"
#include "assetType.h"
#include "fileWatchSystem.h"

namespace dory::core::resources::entity
{
    template<typename T = IdType>
    struct Entity
    {
        using IdType = IdType;

        T id {};

        Entity() = default;

        explicit Entity(T id):
                id(id)
        {}
    };

    struct Camera: public Entity<IdType>
    {
        explicit Camera(IdType id):
                Entity(id)
        {}
    };

    class IWindowSubsystemData
    {
    public:
        virtual ~IWindowSubsystemData() = default;
    };

    struct Window: public Entity<IdType>
    {
        explicit Window(IdType id, WindowSystem windowSystem, GraphicalSystem graphicalSystem):
                Entity(id),
                windowSystem(windowSystem),
                graphicalSystem(graphicalSystem)
        {}

        WindowSystem windowSystem {};
        GraphicalSystem graphicalSystem {};
        std::shared_ptr<IWindowSubsystemData> windowSystemData;
    };

    struct Viewport
    {
        int x {};
        int y {};
        int width {};
        int height {};

        Viewport() = default;

        Viewport(int x, int y, int width, int height):
                x(x),
                y(y),
                width(width),
                height(height)
        {}
    };

    struct View: public Entity<IdType>
    {
        IdType windowId;
        IdType cameraId {};
        std::optional<Viewport> viewport {};

        View(IdType id, IdType windowId, IdType cameraId = nullId):
                Entity(id),
                windowId(windowId),
                cameraId(cameraId)
        {}
    };

    struct PipelineNode: Entity<IdType>
    {
        using UpdateFunctionType = std::function<void(IdType referenceId, const generic::model::TimeSpan& timeStep, DataContext& context)>;
        using ControllerPointerType = std::shared_ptr<void>;

        std::optional<generic::extension::ResourceHandle<ControllerPointerType>> attachedController {};
        std::optional<generic::extension::ResourceHandle<UpdateFunctionType>> updateFunction {};
        IdType parentNodeId {};
        std::string name;

        PipelineNode() = default;

        explicit PipelineNode(IdType id,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(id),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<ControllerPointerType> attachedController,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                attachedController(std::move(attachedController)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}

        explicit PipelineNode(generic::extension::ResourceHandle<UpdateFunctionType> updateFunction,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                updateFunction(std::move(updateFunction)),
                parentNodeId(parentNodeId),
                name(std::move(name))
        {}
    };

    class FileWatchSpecificData
    {
    public:
        virtual ~FileWatchSpecificData() = default;
    };

    struct FileWatch: Entity<IdType>
    {
        std::string path;
        bool recursive;
        AssetType assetType;
        FileWatchSystem fileWatchSystem;
        std::shared_ptr<FileWatchSpecificData> specificData;
    };
}