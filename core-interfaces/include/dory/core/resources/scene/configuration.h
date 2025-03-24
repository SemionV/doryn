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
            using al = Align;

            inline ContainerDefinition def(const Name& name)
            {
                ContainerDefinition result;
                result.name = name;
                return result;
            }

            inline ContainerDefinition def()
            {
                return ContainerDefinition {};
            }

            template <typename Func>
            ContainerDefinition operator|(ContainerDefinition container, Func func)
            {
                return func(container);
            }

            inline void setValue(Dimension& dim, const int value)
            {
                dim.pixels = value;
            }

            inline void setValue(Dimension& dim, const float value)
            {
                dim.percents = value;
            }

            inline void setValue(Dimension& dim, const Name& value)
            {
                dim.variable = value;
            }

            template<typename T>
            void setPoint(DimensionPoint& dim, const T& value)
            {
                setValue(dim, value);
            }

            inline void setPoint(DimensionPoint& dim, const Align value)
            {
                dim.align = value;
            }

            template<typename T>
            void setSize(DimensionSegment& dim, const T& value)
            {
                setValue(dim, value);
            }

            inline void setSize(DimensionSegment& dim, const Upstream value)
            {
                dim.upstream = value;
            }

            template<typename T>
            auto x(const T& value) {
                return [value](ContainerDefinition& container) -> ContainerDefinition& {
                    setPoint(container.x, value);
                    return container;
                };
            }

            template<typename T>
            auto y(const T& value) {
                return [value](ContainerDefinition& container) -> ContainerDefinition& {
                    setPoint(container.y, value);
                    return container;
                };
            }

            template<typename T>
            auto w(const T& value) {
                return [value](ContainerDefinition& container) -> ContainerDefinition& {
                    setSize(container.width, value);
                    return container;
                };
            }

            template<typename T>
            auto h(const T& value) {
                return [value](ContainerDefinition& container) -> ContainerDefinition& {
                    setSize(container.height, value);
                    return container;
                };
            }

            inline auto columns(const std::vector<ContainerDefinition>& items) {
                return [items](ContainerDefinition& container) -> ContainerDefinition& {
                    container.columns = items;
                    return container;
                };
            }

            inline auto rows(const std::vector<ContainerDefinition>& items) {
                return [items](ContainerDefinition& container) -> ContainerDefinition& {
                    container.rows = items;
                    return container;
                };
            }

            inline auto rowTiles(const std::vector<ContainerDefinition>& items) {
                return [items](ContainerDefinition& container) -> ContainerDefinition& {
                    container.tileRow = items;
                    return container;
                };
            }

            inline auto columnTiles(const std::vector<ContainerDefinition>& items) {
                return [items](ContainerDefinition& container) -> ContainerDefinition& {
                    container.tileColumn = items;
                    return container;
                };
            }

            inline auto slides(const std::vector<ContainerDefinition>& items) {
                return [items](ContainerDefinition& container) -> ContainerDefinition& {
                    container.slides = items;
                    return container;
                };
            }

            inline auto floating(const std::vector<ContainerDefinition>& items) {
                return [items](ContainerDefinition& container) -> ContainerDefinition& {
                    container.floating = items;
                    return container;
                };
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