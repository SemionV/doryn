#include <refl.hpp>
#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <magic_enum/magic_enum.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <dory/core/registry.h>
#include <dory/core/resources/serialization.h>
#include <dory/serialization/yamlDeserializer.h>
#include <dory/serialization/yamlSerializer.h>
#include <dory/serialization/jsonDeserializer.h>
#include <dory/serialization/jsonSerializer.h>
#include <dory/serialization/object.h>
#include <dory/core/services/serializer.h>
#include <dory/core/services/iObjectFactory.h>
#include <dory/math/linearAlgebra.h>
#include <spdlog/fmt/bundled/format.h>
#include <dory/core/resources/objectFactoryRefl.h>

using namespace testing;

class SomeController: public dory::core::IController
{
public:
    int parameter {};

    bool initialize(dory::core::resources::IdType nodeId, dory::core::resources::DataContext& context) override
    {
        return true;
    }

    void stop(dory::core::resources::IdType nodeId, dory::core::resources::DataContext& context) override
    {}

    void update(dory::core::resources::IdType nodeId, const dory::generic::model::TimeSpan& timeStep, dory::core::resources::DataContext& context) override
    {
        std::cout << "SomeController::update" << std::endl;
    }
};

REFL_TYPE(SomeController)
    REFL_FIELD(parameter)
REFL_END


struct Controllers
{
    SomeController SomeController;
};

REFL_TYPE(Controllers)
    REFL_FIELD(SomeController)
REFL_END

TEST(ControllerFactory, createByName)
{
    constexpr std::string_view typeName = "SomeController";
    std::shared_ptr<dory::core::IController> controllerInstance;

    auto typeDescriptor = refl::reflect<Controllers>();

    for_each(typeDescriptor.members, [&]<typename MemberDescriptor>(MemberDescriptor memberDescriptor)
    {
        if constexpr (is_field(memberDescriptor))
        {
            const auto memberName = std::string_view{MemberDescriptor::name.data, MemberDescriptor::name.size};
            if(memberName == typeName)
            {
                controllerInstance = std::make_shared<typename MemberDescriptor::value_type>();
            }
        }
    });

    EXPECT_TRUE(controllerInstance != nullptr);
}

enum class Color
{
    Red,
    Green,
    Blue
};

TEST(Enums, reflection)
{
    constexpr Color optionValue = magic_enum::enum_cast<Color>("Green").value();
    constexpr std::string_view optionName = magic_enum::enum_name(Color::Blue);

    EXPECT_EQ(optionValue, Color::Green);
    EXPECT_EQ(optionName, "Blue");
}

enum class ShaderType
{
    unknown,
    vertex,
    fragment
};

struct Shader
{
    std::string filename {};
    ShaderType type {};
};

REFL_TYPE(Shader)
    REFL_FIELD(filename)
    REFL_FIELD(type)
REFL_END

struct Material
{
    std::unordered_map<ShaderType, std::string> shaders {};
};

REFL_TYPE(Material)
    REFL_FIELD(shaders)
REFL_END

using vec2 = std::array<float, 2>;
using vec3 = std::array<float, 3>;
using vec4 = std::array<float, 4>;
using quat = std::array<float, 4>;

struct Transform
{
    vec3 position;
    quat rotation;
    vec3 scale;
};

struct Transform2
{
    glm::vec3 position;
    glm::quat rotation;
};

REFL_TYPE(Transform2)
    REFL_FIELD(position)
    REFL_FIELD(rotation)
REFL_END

REFL_TYPE(Transform)
    REFL_FIELD(position)
    REFL_FIELD(rotation)
    REFL_FIELD(scale)
REFL_END

template<typename TPolicies>
using ObjectVisitorExtensions = dory::core::services::serialization::ObjectVisitorExtensions<TPolicies>;

using SerializationContextPoliciesType = dory::generic::serialization::ContextPolicies<int, int, int>;
int defaultRegistry;
int defaultDataContext;
int defaultDataFormat;
dory::generic::serialization::Context<SerializationContextPoliciesType> serializationContext { defaultRegistry, defaultDataContext, defaultDataFormat };

TEST(YamlDeserialization, deserializeEnumValue)
{
    const auto yaml = R"(
filename: assets/shaders/simpleVertex
type: fragment
)";

    const auto [filename, type] = dory::serialization::yaml::deserialize<Shader>(yaml, std::move(serializationContext));

    EXPECT_EQ(filename, "assets/shaders/simpleVertex");
    EXPECT_EQ(type, ShaderType::fragment);
}

TEST(YamlDeserialization, deserializeGlmVec)
{
    const auto yaml = R"(
position: [0.1, 0.34, 1.89]
rotation: [1.1, 1.34, 2.89, 1.0]
)";

    auto [position, rotation] = dory::serialization::yaml::deserialize<Transform2, SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>>(yaml, std::move(serializationContext));

    EXPECT_EQ(position.x, 0.1f);
    EXPECT_EQ(position.y, 0.34f);
    EXPECT_EQ(position.z, 1.89f);

    EXPECT_EQ(rotation.x, 1.1f);
    EXPECT_EQ(rotation.y, 1.34f);
    EXPECT_EQ(rotation.z, 2.89f);
    EXPECT_EQ(rotation.w, 1.0f);
}

TEST(YamlSerialization, serializeGlmVec)
{
    constexpr Transform2 transform {
        {0.1f, 0.34f, 1.89f},
        {1.1f, 1.34f, 2.89f, 1.0f}
    };

    const auto yaml = dory::serialization::yaml::serialize<Transform2, SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::yaml::YamlSerializationPolicies>>(transform, std::move(serializationContext));

    auto [position, rotation] = dory::serialization::yaml::deserialize<Transform2, SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>>(yaml, std::move(serializationContext));

    EXPECT_EQ(position.x, transform.position.x);
    EXPECT_EQ(position.y, transform.position.y);
    EXPECT_EQ(position.z, transform.position.z);

    EXPECT_EQ(rotation.x, transform.rotation.x);
    EXPECT_EQ(rotation.y, transform.rotation.y);
    EXPECT_EQ(rotation.z, transform.rotation.z);
    EXPECT_EQ(rotation.w, transform.rotation.w);
}

TEST(YamlDeserialization, deserializeInvalidEnumValue)
{
    const auto yaml = R"(
filename: assets/shaders/simpleVertex
type: geometry
)";

    auto shader = dory::serialization::yaml::deserialize<Shader>(yaml, std::move(serializationContext));

    EXPECT_EQ(shader.filename, "assets/shaders/simpleVertex");
    EXPECT_EQ(shader.type, ShaderType::unknown);
}

TEST(YamlMathDeserialization, deserializeVectors)
{
    const auto yaml = R"(
position: [1.0, 2.0, 3.1]
rotation: [3.14546456, 2.2, 1.1, 4.56]
scale: [1.5, 2.1, 3.1]
)";

    const auto transform = dory::serialization::yaml::deserialize<Transform>(yaml, std::move(serializationContext));

    const auto position = dory::math::toVector(transform.position);
    const glm::quat rotation = dory::math::toQuaternion(transform.rotation);
    const auto scale = dory::math::toVector(transform.scale);

    EXPECT_EQ(position.x, 1.0f);
    EXPECT_EQ(position.y, 2.0f);
    EXPECT_EQ(position.z, 3.1f);

    EXPECT_EQ(rotation.w, 3.14546456f);
    EXPECT_EQ(rotation.x, 2.2f);
    EXPECT_EQ(rotation.y, 1.1f);
    EXPECT_EQ(rotation.z, 4.56f);

    EXPECT_EQ(scale.x, 1.5f);
    EXPECT_EQ(scale.y, 2.1f);
    EXPECT_EQ(scale.z, 3.1f);
}

TEST(YamlSerialization, serializeEnumValue)
{
    const auto shader = Shader { "assets/shaders/simpleVertex", ShaderType::fragment };
    const auto yaml = dory::serialization::yaml::serialize(shader, std::move(serializationContext));

    auto [filename, type] = dory::serialization::yaml::deserialize<Shader>(yaml, std::move(serializationContext));

    EXPECT_EQ(filename, shader.filename);
    EXPECT_EQ(type, shader.type);
}

TEST(YamlDeserialization, deserializeDictionaryWithEnumKeys)
{
    const auto yaml = R"(
shaders:
  vertex: vertexShader
  fragment: fragmentShader
)";

    auto [shaders] = dory::serialization::yaml::deserialize<Material>(yaml, std::move(serializationContext));

    EXPECT_EQ(shaders.size(), 2);
    EXPECT_EQ(shaders[ShaderType::vertex], "vertexShader");
    EXPECT_EQ(shaders[ShaderType::fragment], "fragmentShader");
}

TEST(YamlSerialization, serializeDictionaryWithEnumKeys)
{
    const auto material = Material {
            {
                { ShaderType::vertex, "vertexShader" },
                { ShaderType::fragment, "fragmentShader" }
            }
    };

    const auto yaml = dory::serialization::yaml::serialize(material, std::move(serializationContext));

    auto [shaders] = dory::serialization::yaml::deserialize<Material>(yaml, std::move(serializationContext));

    EXPECT_EQ(shaders.size(), material.shaders.size());
    EXPECT_EQ(shaders[ShaderType::vertex], material.shaders.at(ShaderType::vertex));
    EXPECT_EQ(shaders[ShaderType::fragment], material.shaders.at(ShaderType::fragment));
}

TEST(JsonDeserialization, deserializeEnumValue)
{
    const auto json = R"({
"filename": "assets/shaders/simpleVertex",
"type": "fragment"
})";

    auto [filename, type] = dory::serialization::json::deserialize<Shader>(json, std::move(serializationContext));

    EXPECT_EQ(filename, "assets/shaders/simpleVertex");
    EXPECT_EQ(type, ShaderType::fragment);
}

TEST(JsonDeserialization, deserializeGlmVec)
{
    const auto json = R"({
"position": [0.1, 0.34, 1.89],
"rotation": [1.1, 1.34, 2.89, 1.0]
})";

    auto [position, rotation] = dory::serialization::json::deserialize<Transform2, SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>>(json, std::move(serializationContext));

    EXPECT_EQ(position.x, 0.1f);
    EXPECT_EQ(position.y, 0.34f);
    EXPECT_EQ(position.z, 1.89f);

    EXPECT_EQ(rotation.x, 1.1f);
    EXPECT_EQ(rotation.y, 1.34f);
    EXPECT_EQ(rotation.z, 2.89f);
    EXPECT_EQ(rotation.w, 1.0f);
}

TEST(JsonSerialization, serializeGlmVec)
{
    constexpr Transform2 transform {
            {0.1f, 0.34f, 1.89f},
            {1.1f, 1.34f, 2.89f, 1.0f}
    };

    const auto json = dory::serialization::json::serialize<Transform2, SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::json::JsonSerializationPolicies>>(transform, std::move(serializationContext));

    auto [position, rotation] = dory::serialization::json::deserialize<Transform2, SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::json::JsonDeserializationPolicies>>(json, std::move(serializationContext));

    EXPECT_EQ(position.x, transform.position.x);
    EXPECT_EQ(position.y, transform.position.y);
    EXPECT_EQ(position.z, transform.position.z);

    EXPECT_EQ(rotation.x, transform.rotation.x);
    EXPECT_EQ(rotation.y, transform.rotation.y);
    EXPECT_EQ(rotation.z, transform.rotation.z);
    EXPECT_EQ(rotation.w, transform.rotation.w);
}

TEST(JsonDeserialization, deserializeInvalidEnumValue)
{
    const auto json = R"({
"filename": "assets/shaders/simpleVertex",
"type": "geometry"
})";

    auto [filename, type] = dory::serialization::json::deserialize<Shader>(json, std::move(serializationContext));

    EXPECT_EQ(filename, "assets/shaders/simpleVertex");
    EXPECT_EQ(type, ShaderType::unknown);
}

TEST(JsonSerialization, serializeEnumValue)
{
    const auto shader = Shader { "assets/shaders/simpleVertex", ShaderType::fragment };
    const auto json = dory::serialization::json::serialize(shader, std::move(serializationContext));

    auto [filename, type] = dory::serialization::json::deserialize<Shader>(json, std::move(serializationContext));

    EXPECT_EQ(filename, shader.filename);
    EXPECT_EQ(type, shader.type);
}

TEST(JsonDeserialization, deserializeDictionaryWithEnumKeys)
{
    const auto json = R"({
"shaders": {
  "vertex": "vertexShader",
  "fragment": "fragmentShader"
}})";

    auto [shaders] = dory::serialization::json::deserialize<Material>(json, std::move(serializationContext));

    EXPECT_EQ(shaders.size(), 2);
    EXPECT_EQ(shaders[ShaderType::vertex], "vertexShader");
    EXPECT_EQ(shaders[ShaderType::fragment], "fragmentShader");
}

TEST(JsonSerialization, serializeDictionaryWithEnumKeys)
{
    auto material = Material {
            {
                { ShaderType::vertex, "vertexShader" },
                { ShaderType::fragment, "fragmentShader" }
            }
    };

    const auto json = dory::serialization::json::serialize(material, std::move(serializationContext));

    auto [shaders] = dory::serialization::json::deserialize<Material>(json, std::move(serializationContext));

    EXPECT_EQ(shaders.size(), material.shaders.size());
    EXPECT_EQ(shaders[ShaderType::vertex], material.shaders[ShaderType::vertex]);
    EXPECT_EQ(shaders[ShaderType::fragment], material.shaders[ShaderType::fragment]);
}

enum class FaceRendering
{
    solid,
    wireframe
};

struct Uniforms
{
    std::optional<std::array<float, 4>> color;
    std::unordered_map<ShaderType, std::string> shaders;
};

struct Material2
{
    std::optional<FaceRendering> face;
    std::string name {};
    std::vector<std::string> baseMaterials;
    Uniforms uniforms;
    int id {};
};

REFL_TYPE(Uniforms)
    REFL_FIELD(color)
    REFL_FIELD(shaders)
REFL_END

REFL_TYPE(Material2)
    REFL_FIELD(face)
    REFL_FIELD(name)
    REFL_FIELD(baseMaterials)
    REFL_FIELD(uniforms)
    REFL_FIELD(id)
REFL_END

TEST(ObjectCopy, copyObjects)
{
    auto material = Material2 {};
    material.name = "destination";
    material.face = FaceRendering::wireframe;
    material.baseMaterials = { "mat1" };
    material.uniforms.shaders[ShaderType::vertex] = "vertexShader";

    auto materialBase = Material2 {};
    materialBase.name = "source";
    materialBase.baseMaterials = { "mat2" };
    materialBase.uniforms.color = { 0.1f, 0.2f, 0.3f, 1.f };
    materialBase.uniforms.shaders[ShaderType::fragment] = "fragmentShader";
    materialBase.id = 2;

    dory::serialization::object::copy(materialBase, material, std::move(serializationContext));

    EXPECT_EQ(material.face, FaceRendering::wireframe);
    EXPECT_EQ(material.name, "source");
    EXPECT_EQ(material.id, 2);
    EXPECT_EQ(material.baseMaterials.size(), 2);
    EXPECT_EQ(material.baseMaterials[0], "mat1");
    EXPECT_EQ(material.baseMaterials[1], "mat2");
    EXPECT_EQ(material.uniforms.color.has_value(), true);
    auto color = material.uniforms.color.value();
    EXPECT_EQ(color[0], 0.1f);
    EXPECT_EQ(color[1], 0.2f);
    EXPECT_EQ(color[2], 0.3f);
    EXPECT_EQ(color[3], 1.f);
    EXPECT_EQ(material.uniforms.shaders.size(), 2);
    EXPECT_EQ(material.uniforms.shaders[ShaderType::vertex], "vertexShader");
    EXPECT_EQ(material.uniforms.shaders[ShaderType::fragment], "fragmentShader");
}

struct PipelineNode
{
    dory::core::resources::serialization::FactoryInstance<dory::core::IController> controller;
};

REFL_TYPE(PipelineNode)
    REFL_FIELD(controller)
REFL_END

template<typename T>
class ObjectFactoryMock final : public dory::core::services::IObjectFactory<T>
{
public:
    using SerializationContextType = dory::generic::serialization::Context<dory::core::services::serialization::SerializationContextPoliciesType>;
    MOCK_METHOD(dory::generic::extension::ResourceHandle<std::shared_ptr<T>>, createInstance, (SerializationContextType& context), (final));
};

TEST(ObjectFactory, createInstanceFromYaml)
{
    const auto yaml = R"(
controller:
  type: SomeController
  parameter: 4
)";

    dory::core::Registry registry{};
    const dory::generic::extension::LibraryHandle libraryHandle {};
    dory::core::resources::configuration::Configuration configuration {};
    dory::core::resources::Localization localization {};
    dory::core::resources::DataContext context { configuration, localization};
    auto someController = std::make_shared<SomeController>();
    someController->parameter = 4;
    auto someControllerHandle = dory::generic::extension::ResourceHandle<std::shared_ptr<dory::core::IController>> { libraryHandle, someController };

    const auto controllerFactory = std::make_shared<ObjectFactoryMock<dory::core::IController>>();
    registry.set<dory::core::services::IObjectFactory<dory::core::IController>>(libraryHandle, controllerFactory, std::string { "SomeController" });

    using SerializationContextType = ObjectFactoryMock<dory::core::IController>::SerializationContextType;
    const auto serializationContextMatcher = Truly([&](SerializationContextType& serializationContext) {
        const auto& yamlDeserializationContext = static_cast<dory::serialization::yaml::YamlContext<dory::core::services::serialization::SerializationContextPoliciesType>&>(serializationContext);
        const auto current = yamlDeserializationContext.node;
        const bool yamlContextIsCorrect = current.is_map() && current.has_child("parameter") && current.has_child("type");

        return yamlContextIsCorrect && &serializationContext.registry == &registry && &serializationContext.dataContext == &context;
    });
    EXPECT_CALL(*controllerFactory, createInstance(serializationContextMatcher)).WillOnce(Return(someControllerHandle));

    auto contextBase = dory::generic::serialization::Context<dory::core::services::serialization::SerializationContextPoliciesType>{ registry, context, dory::core::resources::DataFormat::yaml };
    auto [controller] = dory::serialization::yaml::deserialize<PipelineNode, dory::core::services::serialization::SerializationContextPoliciesType,
        ObjectVisitorExtensions<dory::serialization::yaml::YamlDeserializationPolicies>>(yaml, std::move(contextBase));

    EXPECT_TRUE(*controller.instance.lock());
}