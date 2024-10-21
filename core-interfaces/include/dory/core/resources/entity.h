#pragma once

#include <memory>
#include <string>
#include <functional>
#include <utility>
#include <type_traits>

namespace dory::core::resources::entity
{
    enum UnitScale
    {
        Identity = 1,
        Milli = 1000,
        Micro = 1000000,
        Nano = 1000000000
    };

    struct TimeSpan
    {
        long duration;
        UnitScale unitsPerSecond;

        TimeSpan():
                duration(0),
                unitsPerSecond(UnitScale::Milli)
        {
        };

        explicit TimeSpan(UnitScale unitsPerSecond):
                duration(0),
                unitsPerSecond(unitsPerSecond)
        {
        };

        [[nodiscard]] double ToMilliseconds() const
        {
            return (duration / (double)unitsPerSecond) * (double)UnitScale::Milli;
        }
    };

    class TimeConverter
    {
    public:
        static double ToMilliseconds(TimeSpan timeSpan)
        {
            return (timeSpan.duration / (double)timeSpan.unitsPerSecond) * (double)UnitScale::Milli;
        }
    };

    struct Color
    {
        float r {1.0f};
        float g {1.0f};
        float b {1.0f};
        float a {1.0f};

        Color() = default;

        Color(float r, float g, float b, float a = 1.0f):
                r(r), g(g), b(b), a(a)
        {}
    };

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

    struct Camera: public Entity<IdType>
    {
        explicit Camera(IdType id):
                Entity(id)
        {}
    };

    enum class WindowSubsystemType
    {
        GLFW
    };

    class IWindowSubsystemData
    {
    public:
        virtual ~IWindowSubsystemData() = default;
    };

    struct Window: public Entity<IdType>
    {
        explicit Window(IdType id, WindowSubsystemType subsystemType):
                Entity(id),
                subsystemType(subsystemType)
        {}

        WindowSubsystemType subsystemType {};
        std::shared_ptr<IWindowSubsystemData> subsystemData;
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
        IdType controllerNodeId;
        IdType cameraId;
        Viewport viewport;

        View(IdType id, IdType windowId, IdType controllerNodeId, IdType cameraId, Viewport viewport):
                Entity(id),
                windowId(windowId),
                controllerNodeId(controllerNodeId),
                cameraId(cameraId),
                viewport(viewport)
        {}
    };

    enum class PipelineNodePriority
    {
        Default = 0,
        First = 1
    };

    template<typename TDataContext>
    struct PipelineNode: Entity<IdType>
    {
        using UpdateFunctionType = std::function<void(IdType referenceId, const TimeSpan& timeStep, TDataContext& context)>;

        std::shared_ptr<void> attachedController;
        UpdateFunctionType update;
        IdType parentNodeId;
        std::string name;
        PipelineNodePriority priority;

        explicit PipelineNode(IdType id,
                              std::shared_ptr<void> attachedController = nullptr,
                              PipelineNodePriority priority = PipelineNodePriority::Default,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(id),
                attachedController(std::move(attachedController)),
                parentNodeId(parentNodeId),
                priority(priority),
                name(std::move(name))
        {}

        template<typename F, typename T = std::enable_if_t<std::is_convertible_v<F, UpdateFunctionType>>>
        explicit PipelineNode(F&& update,
                              PipelineNodePriority priority = PipelineNodePriority::Default,
                              IdType parentNodeId = nullId,
                              std::string name = ""):
                Entity(nullId),
                update(std::forward<F>(update)),
                attachedController(nullptr),
                parentNodeId(parentNodeId),
                priority(priority),
                name(std::move(name))
        {}
    };
}