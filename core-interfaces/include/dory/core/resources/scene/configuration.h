#pragma once

#include <unordered_map>
#include <vector>
#include <dory/core/resources/configuration.h>
#include <dory/core/resources/assets/shader.h>
#include <dory/core/resources/assets/material.h>
#include <dory/core/resources/scene/components.h>
#include <dory/core/resources/factory.h>
#include <dory/core/resources/graphicalSystem.h>
#include <dory/core/iController.h>
#include <dory/core/devices/iDevice.h>
#include <dory/core/services/iSceneDirector.h>

namespace dory::core::resources::scene::configuration
{
    struct Shader
    {
        assets::ShaderType type {};
        std::string filename {};
    };

    struct MaterialProperties
    {
        std::optional<glm::vec4> color {};
    };

    struct Material
    {
        std::vector<std::string> baseMaterials {};
        std::vector<std::string> shaders {};
        MaterialProperties properties {};
        std::optional<assets::PolygonMode> polygonMode {};
    };

    struct Mesh
    {
        std::string filename {};
    };

    struct Assets
    {
        std::unordered_map<std::string, Shader> shaders {};
        std::unordered_map<std::string, Material> materials {};
        std::unordered_map<std::string, Mesh> meshes {};
    };

    struct ObjectComponents
    {
        std::optional<components::LinearMovement> linearMovement {};
        std::optional<components::RotationMovement> rotationMovement {};
    };

    struct Transform
    {
        glm::vec3 position {};
        glm::quat rotation {};
        glm::vec3 scale {};
    };

    struct Object
    {
        Transform transform {};
        std::string mesh {};
        std::string material {};
        ObjectComponents components;
        std::unordered_map<std::string, Object> children {};
    };

    struct Node
    {
        IdType pipelineNodeId {};
        std::string parent {};
        factory::Instance<ITrigger> trigger {};
        factory::Instance<IController> controllerInstance {};
        std::string controller {};
        std::unordered_map<std::string, Node> children {};
    };

    struct Pipeline
    {
        std::unordered_map<std::string, Node> nodes {};
    };

    struct GraphicalContext
    {
        GraphicalSystem graphicalSystem {};
    };

    namespace layout
    {
        enum class Align
        {
            center
        };

        enum class Upstream
        {
            parent,
            fill,
            children
        };

        struct Dimension
        {
            std::optional<int> pixels {};
            std::optional<std::string> variable {};
            std::optional<float> percents {};
        };

        struct DimensionPoint: public Dimension
        {
            std::optional<Align> align {};
        };

        struct DimensionSegment: public Dimension
        {
            std::optional<Upstream> upstream {};
        };

        struct ContainerDefinition
        {
            DimensionSegment width;
            DimensionSegment height;
            DimensionPoint x;
            DimensionPoint y;
            std::string name {};

            std::vector<ContainerDefinition> columns {};
            std::vector<ContainerDefinition> rows {};
            std::vector<ContainerDefinition> tileRow {};
            std::vector<ContainerDefinition> tileColumn {};
            std::vector<ContainerDefinition> floating {};
            std::vector<ContainerDefinition> slides {};

            DimensionPoint& getPosition(const std::size_t axis)
            {
                if(axis == 0)
                {
                    return x;
                }

                return y;
            }

            DimensionSegment& getSize(const std::size_t axis)
            {
                if(axis == 0)
                {
                    return width;
                }

                return height;
            }

            ContainerDefinition() = default;

            explicit ContainerDefinition(const std::string& name, const DimensionPoint& x, const DimensionPoint& y,
                const DimensionSegment& width, const DimensionSegment& height):
                width(width), height(height), x(x), y(y), name(name)
            {}
        };

        using DimensionSegmentProperty = DimensionSegment ContainerDefinition::*;
        using DimensionPointProperty = DimensionSegment ContainerDefinition::*;

        namespace util
        {
            using us = Upstream;

            inline DimensionSegment segment(const int pixels)
            {
                auto result = DimensionSegment{};
                result.pixels = pixels;
                return result;
            }

            inline DimensionSegment segment(const float percents)
            {
                auto result = DimensionSegment{};
                result.percents = percents;
                return result;
            }

            inline DimensionSegment segment(const std::string& variable)
            {
                auto result = DimensionSegment{};
                result.variable = variable;
                return result;
            }

            inline DimensionSegment segment(const Upstream upstream)
            {
                auto result = DimensionSegment{};
                result.upstream = upstream;
                return result;
            }

            inline DimensionSegment segment()
            {
                return DimensionSegment{};
            }

            inline DimensionPoint point(const int pixels)
            {
                auto result = DimensionPoint{};
                result.pixels = pixels;
                return result;
            }

            inline DimensionPoint point(const float percents)
            {
                auto result = DimensionPoint{};
                result.percents = percents;
                return result;
            }

            inline DimensionPoint point(const std::string& variable)
            {
                auto result = DimensionPoint{};
                result.variable = variable;
                return result;
            }

            inline DimensionPoint point(const Align alignTo)
            {
                return DimensionPoint{ {}, alignTo };
            }

            inline DimensionPoint point()
            {
                return DimensionPoint{};
            }

            inline ContainerDefinition define(const std::string& name, const DimensionPoint& x, const DimensionPoint& y, const DimensionSegment& width, const DimensionSegment& height)
            {
                ContainerDefinition result {name, x, y, width, height};
                return result;
            }

            inline ContainerDefinition define(const std::string& name, const DimensionSegment& width, const DimensionSegment& height)
            {
                ContainerDefinition result {name, {}, {}, width, height};
                return result;
            }

            inline ContainerDefinition defineRow(const std::string& name, const DimensionSegment& width, const DimensionSegment& height, const std::vector<ContainerDefinition>& columns)
            {
                ContainerDefinition result {name, {}, {}, width, height};
                result.columns = columns;

                return result;
            }

            inline ContainerDefinition defineColumn(const std::string& name, const DimensionSegment& width, const DimensionSegment& height, const std::vector<ContainerDefinition>& rows)
            {
                ContainerDefinition result {name, {}, {}, width, height};
                result.rows = rows;

                return result;
            }

            template<typename... T>
            ContainerDefinition def(const T& ...value)
            {
                return define(value...);
            }

            inline ContainerDefinition row(const std::string& name, const DimensionSegment& width, const DimensionSegment& height, const std::vector<ContainerDefinition>& columns)
            {
                return defineRow(name, width, height, columns);
            }

            inline ContainerDefinition column(const std::string& name, const DimensionSegment& width, const DimensionSegment& height, const std::vector<ContainerDefinition>& rows)
            {
                return defineColumn(name, width, height, rows);
            }

            template<typename... T>
            ContainerDefinition column(const T& ...value, std::initializer_list<ContainerDefinition> children = {})
            {
                return defineColumn(value..., children);
            }

            template<typename... T>
            DimensionSegment seg(const T& ...value)
            {
                return segment(value...);
            }

            template<typename... T>
            DimensionSegment w(const T& ...value)
            {
                return seg(value...);
            }

            template<typename... T>
            DimensionSegment h(const T& ...value)
            {
                return seg(value...);
            }

            template<typename... T>
            DimensionPoint pt(const T& ...value)
            {
                return point(value...);
            }

            template<typename... T>
            DimensionPoint x(const T& ...value)
            {
                return pt(value...);
            }

            template<typename... T>
            DimensionPoint y(const T& ...value)
            {
                return pt(value...);
            }
        }
    }

    struct Window
    {
        std::string title {};
        std::string graphicalContext {};
    };

    struct View
    {
        std::string container; //attach to a grid column
    };

    struct SceneConfiguration: Entity<>
    {
        std::string name;
        Assets assets;
        std::unordered_map<std::string, Object> objects {};
        std::unordered_map<std::string, std::string> cameras {};
        std::unordered_map<std::string, std::string> lights {};
        Pipeline pipeline;
        std::vector<resources::configuration::Extension> extensions;
        std::vector<factory::Instance<devices::IDevice>> devices;
        std::vector<factory::Instance<services::ISceneDirector>> directors;
        std::unordered_map<std::string, GraphicalContext> graphicalContexts {};
        std::unordered_map<std::string, Window> windows {};
    };
}