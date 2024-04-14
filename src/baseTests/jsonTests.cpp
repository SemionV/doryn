#include "dependencies.h"
#include "base/serialization/jsonDeserializer.h"

using json = nlohmann::json;

TEST_CASE( "Basic", "[json]" )
{
    json ex1 = json::parse(R"(
    {
        "pi": 3.141,
        "happy": true
    })");

    REQUIRE(ex1["happy"] == true);

    auto happy = ex1["happy"];

    if(ex1.is_object())
    {
        auto& propertyName = ex1.items().begin().key();
    }
}

TEST_CASE( "Basic collection", "[.][json]" )
{
    json ex1 = json::parse(R"(
    [
        1,
        2
    ])");

    if(ex1.is_array())
    {
        for(auto i = 0; i < ex1.size(); ++i)
        {
            auto& itemJson = ex1.at(i);
            auto value = itemJson.get<int>();

            std::cout << value << "\n";
        }
    }
}

struct Mesh
{
    std::vector<std::array<float, 3>> vertices;
};

REFL_TYPE(Mesh)
    REFL_FIELD(vertices)
REFL_END

REFL_TYPE(std::optional<Mesh>)
REFL_END

struct Entity
{
    std::string name;
    std::array<float, 3> position;
    std::optional<Mesh> mesh;
};

REFL_TYPE(Entity)
        REFL_FIELD(name)
        REFL_FIELD(position)
        REFL_FIELD(mesh)
REFL_END

struct Scene
{
    std::string name;
    std::vector<Entity> entities;
};

REFL_TYPE(std::vector<Entity>)
REFL_END

REFL_TYPE(Scene)
        REFL_FIELD(name)
        REFL_FIELD(entities)
REFL_END

TEST_CASE( "Deserialize vector of objects", "[json]" )
{
    std::string meshJson = R"(
    {
        "vertices": [
            [1, 1, 1],
            [2, 2, 2]
        ]
    })";

    auto mesh = dory::typeMap::json::JsonDeserializer::deserialize<Mesh>(meshJson);
    REQUIRE(mesh.vertices.size() == 2);
    REQUIRE(mesh.vertices[0][0] == 1);
    REQUIRE(mesh.vertices[0][1] == 1);
    REQUIRE(mesh.vertices[0][2] == 1);
    REQUIRE(mesh.vertices[1][0] == 2);
    REQUIRE(mesh.vertices[1][1] == 2);
    REQUIRE(mesh.vertices[1][2] == 2);
}