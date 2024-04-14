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

struct Vector
{
    float x;
    float y;
    float z;
};

REFL_TYPE(Vector)
    REFL_FIELD(x)
    REFL_FIELD(y)
    REFL_FIELD(z)
REFL_END

REFL_TYPE(std::vector<Vector>)
REFL_END

struct Mesh
{
    std::vector<Vector> vertecies;
};

REFL_TYPE(Mesh)
    REFL_FIELD(vertecies)
REFL_END

struct Entity
{
    std::string name;
    Vector position;
    std::optional<Mesh> mesh;
};

struct Scene
{
    std::string name;
    std::vector<Entity> entities;
};

TEST_CASE( "Deserialize vector of objects", "[json]" )
{
    std::string meshJson = R"(
    [
        {"x": 1, "y": 1, "z": 1},
        {"x": 2, "y": 2, "z": 2}
    ])";

    auto mesh = dory::typeMap::json::JsonDeserializer::deserialize<Mesh>(meshJson);
    REQUIRE(mesh.vertecies.size() == 2);
}