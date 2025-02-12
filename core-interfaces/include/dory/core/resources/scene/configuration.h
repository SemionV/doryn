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
        IdType pipelineNodeId;
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
        GraphicalSystem graphicalSystem;
    };

    namespace layout
    {
        struct Dimension
        {
            std::optional<int> pixels{};
            std::optional<float> percents{};
        };

        struct Position
        {
            std::optional<Dimension> x{};
            std::optional<Dimension> y{};
        };

        struct Size
        {
            std::optional<Dimension> width{}; //if it has no value use content width
            std::optional<Dimension> height{}; //if it has no value use content height
        };

        struct Container;

        struct Container
        {
            std::string name;
            std::optional<Position> position {};
            std::optional<Size> size {}; //if it has no value use container's available space
            std::vector<Container> containers;
        };
    }

    struct Window
    {
        layout::Container container;
        std::string title;
        std::string graphicalContext;
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

        //fullscreen window with a centered box
        Window window1 = {
            {
                "window1",
                std::optional<layout::Position> {}, //auto, stick to the left upper corner of the available space: {0,0}
                std::optional<layout::Size> {}, //auto: use all available screen space
                {
                    layout::Container{
                        "dialog",
                        layout::Position {}, //auto: centered rectangle according to rectangle size and available space
                        layout::Size { layout::Dimension{800}, layout::Dimension{600} }
                    }
                }
            }
        };

        //relatively placed window of flexible height with a grid layout
        Window window2 = {
            {
                "window2",
                layout::Position {
                    layout::Dimension{100}/*100px offset from the left*/,
                    {}/*no value - centered by height of the screen*/
                },
                layout::Size { layout::Dimension{1024}, layout::Dimension{}/*Use height of the children*/},
                {
                    {
                        "row1", {}, layout::Size{ {}/*available width of the parent*/, layout::Dimension{100} },
                        {
                            { "cell_11", {}, layout::Size{ layout::Dimension{{}, {40.3f}}, {}/*height of the parent*/ }},
                            { "cell_12", {}, {} },
                            { "cell_13", {}, {} }
                        }
                    },
                    {
                        "row2", {}, layout::Size{ {}/*available width of the parent*/, layout::Dimension{100} },
                        {
                            { "cell_21", {}, layout::Size{ layout::Dimension{{}, {40.3f}}, {}/*height of the parent*/ }},
                            { "cell_22", {}, {} },
                            { "cell_23", {}, {} }
                        }
                    },
                    {
                        "row3", {}, layout::Size{ {}/*available width of the parent*/, layout::Dimension{100} },
                        {
                            { "cell_31", {}, layout::Size{ layout::Dimension{{}, {40.3f}}, {}/*height of the parent*/ }},
                            { "cell_32", {}, {} },
                            { "cell_33", {}, {} }
                        }
                    }
                }
            }
        };

        Window window3 = {
        };

        Window window4 = {
            {
                "window4",
                layout::Position {}, //auto: centered
                layout::Size {}, //auto: size of content
                {
                    {
                        "container with vertical scroll",
                        std::optional<layout::Position> {}, //auto, stick to the left upper corner of the available space: {0,0}
                        layout::Size { layout::Dimension{1024}, layout::Dimension{768} },
                        {
                            {
                                "very tall content container",
                                std::optional<layout::Position> {},
                                layout::Size {
                                    {}/*use width of the parent if children do not provide fixed size*/,
                                    layout::Dimension{3000} /*taller than the parent, reason for scrolls*/
                                }
                            }
                        }
                    }
                }
            }
        };
    };
}