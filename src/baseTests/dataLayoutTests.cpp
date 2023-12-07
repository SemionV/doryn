#include "baseTests/dependencies.h"
#include "templates/structures/dataLayout.h"

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

struct Axes
{
    Point i;
    Point j;
    Point k;
};

REFL_TYPE(Point)
    REFL_FIELD(x)
    REFL_FIELD(y)
    REFL_FIELD(z)
REFL_END

REFL_TYPE(Color)
    REFL_FIELD(r)
    REFL_FIELD(g)
    REFL_FIELD(b)
    REFL_FIELD(a)
REFL_END

REFL_TYPE(Axes)
    REFL_FIELD(i)
    REFL_FIELD(j)
    REFL_FIELD(k)
REFL_END

enum class AttributeId
{
    meshId,
    position,
    color,
    axes,
    normal
};

template<auto Id, typename T>
using Attribute = dory::dataLayout::Attribute<Id, T>;

TEST_CASE( "Layout size check", "[dataLayout]" )
{
    using LayoutMap = dory::dataLayout::Layout<
            Attribute<AttributeId::meshId, std::size_t>,
            Attribute<AttributeId::position, Point>,
            Attribute<AttributeId::color, Color>,
            Attribute<AttributeId::axes, Axes>,
            Attribute<AttributeId::normal, Point>>;

    auto expectedSize = sizeof(std::size_t)
            + sizeof(float) * 3
            + sizeof(float) * 4
            + sizeof(float) * 3 * 3
            + sizeof(float) * 3;

    REQUIRE(dory::dataLayout::LayoutSizeV<LayoutMap> == expectedSize);
}