#include <catch2/catch_test_macros.hpp>
#include <refl.hpp>

#include <dory/serialization/structures/dataLayout.h>

struct Point
{
    float x {};
    float y {};
    float z {};
};

struct Color
{
    float r {};
    float g {};
    float b {};
    float a {};
};

struct Axises
{
    Point i;
    Point j;
    Point k;
};

struct AffineTransformation
{
    Point translation;
    std::array<float, 9> rotation{};
    std::array<Point, 3> axises{};
};

REFL_TYPE(Point)
    REFL_FIELD(x)
    REFL_FIELD(y)
    REFL_FIELD(z)
REFL_END

const std::size_t pointFieldCount = 3;
const std::size_t pointSize = sizeof(float) * pointFieldCount;

REFL_TYPE(Color)
    REFL_FIELD(r)
    REFL_FIELD(g)
    REFL_FIELD(b)
    REFL_FIELD(a)
REFL_END

const std::size_t colorFieldCount = 4;
const std::size_t colorSize = sizeof(float) * colorFieldCount;

REFL_TYPE(Axises)
    REFL_FIELD(i)
    REFL_FIELD(j)
    REFL_FIELD(k)
REFL_END

const std::size_t axisesFieldCount = pointFieldCount * 3;
const std::size_t axisesSize = pointSize * 3;

REFL_TYPE(AffineTransformation)
        REFL_FIELD(translation)
        REFL_FIELD(rotation)
        REFL_FIELD(axises)
REFL_END

const std::size_t affineTransformationFieldCount = pointFieldCount * 3 + 9 + pointFieldCount;
const std::size_t affineTransformationSize = pointSize  + sizeof(float) * 9 + pointSize * 3;

const std::size_t meshIdSize = sizeof(std::size_t);
const std::size_t meshIdFieldCount = 1;

enum class AttributeId
{
    meshId,
    position,
    color,
    axises,
    normal,
    transformation
};

template<auto Id, typename T>
using Attribute = dory::serialization::Attribute<Id, T>;

using LayoutMap = dory::serialization::Layout<
        Attribute<AttributeId::meshId, std::size_t>,
        Attribute<AttributeId::position, Point>,
        Attribute<AttributeId::color, Color>,
        Attribute<AttributeId::axises, Axises>,
        Attribute<AttributeId::normal, Point>,
        Attribute<AttributeId::transformation, AffineTransformation>>;

TEST_CASE( "Layout size check", "[dataLayout]" )
{
    auto expectedSize = meshIdSize
            + pointSize
            + colorSize
            + axisesSize
            + pointSize
            + affineTransformationSize;

    REQUIRE(dory::serialization::LayoutSizeV<LayoutMap> == expectedSize);
}

TEST_CASE( "Layout attributes count check", "[dataLayout]" )
{
    REQUIRE(dory::serialization::LayoutCountV<LayoutMap> == 6);
}

TEST_CASE( "Layout attributes size check", "[dataLayout]" )
{
    REQUIRE(dory::serialization::LayoutAttributeSizeV<AttributeId::meshId, LayoutMap> == meshIdSize);
    REQUIRE(dory::serialization::LayoutAttributeSizeV<AttributeId::position, LayoutMap> == pointSize);
    REQUIRE(dory::serialization::LayoutAttributeSizeV<AttributeId::color, LayoutMap> == colorSize);
    REQUIRE(dory::serialization::LayoutAttributeSizeV<AttributeId::axises, LayoutMap> == axisesSize);
    REQUIRE(dory::serialization::LayoutAttributeSizeV<AttributeId::normal, LayoutMap> == pointSize);
    REQUIRE(dory::serialization::LayoutAttributeSizeV<AttributeId::transformation, LayoutMap> == affineTransformationSize);
}

TEST_CASE( "Layout attributes type check", "[dataLayout]" )
{
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<AttributeId::meshId, LayoutMap>, std::size_t>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<AttributeId::position, LayoutMap>, Point>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<AttributeId::color, LayoutMap>, Color>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<AttributeId::axises, LayoutMap>, Axises>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<AttributeId::normal, LayoutMap>, Point>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<AttributeId::transformation, LayoutMap>, AffineTransformation>);
}

TEST_CASE( "Layout attributes field type check", "[dataLayout]" )
{
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<AttributeId::meshId, LayoutMap>, std::size_t>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<AttributeId::position, LayoutMap>, float>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<AttributeId::color, LayoutMap>, float>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<AttributeId::axises, LayoutMap>, float>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<AttributeId::normal, LayoutMap>, float>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<AttributeId::transformation, LayoutMap>, float>);
}

TEST_CASE( "Layout attributes field count check", "[dataLayout]" )
{
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<AttributeId::meshId, LayoutMap> == meshIdFieldCount);
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<AttributeId::position, LayoutMap> == pointFieldCount);
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<AttributeId::color, LayoutMap> == colorFieldCount);
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<AttributeId::axises, LayoutMap> == axisesFieldCount);
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<AttributeId::normal, LayoutMap> == pointFieldCount);
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<AttributeId::transformation, LayoutMap> == affineTransformationFieldCount);
}

TEST_CASE( "Layout attributes offset check", "[dataLayout]" )
{
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<AttributeId::meshId, LayoutMap> == 0);
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<AttributeId::position, LayoutMap> == meshIdSize);
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<AttributeId::color, LayoutMap> == meshIdSize + pointSize);
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<AttributeId::axises, LayoutMap> == meshIdSize + pointSize + colorSize);
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<AttributeId::normal, LayoutMap> == meshIdSize + pointSize + colorSize + axisesSize);
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<AttributeId::transformation, LayoutMap> == meshIdSize + pointSize + colorSize + axisesSize + pointSize);
}