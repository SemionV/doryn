#include "dependencies.h"

struct Point
{
    int x {};
    int y {};
    int z {};
};

struct Color
{
    int r {};
    int g {};
    int b {};
};

struct TextureCoordinates
{
    int u {};
    int v {};
};

enum class AttributeId
{
    position,
    color,
    textureCoordinates,
    normal,
    meshIndex
};

using AttributeIndex = unsigned short;
using Byte = std::uint8_t;

template<typename TId, TId Id, typename T>
struct Attribute
{
    static constexpr TId id = Id;
    using ValueType = T;
};

template <typename TId, class... Ts>
struct Layout;

template<typename TId>
struct Layout<TId>
{
};

template<typename TId, typename T, typename... Ts>
struct Layout<TId, T, Ts...>: public Layout<TId, Ts...>
{
    using ParentType = Layout<TId, Ts...>;

    static constexpr std::size_t getAttributeSize()
    {
        return sizeof(typename T::ValueType);
    }

    static constexpr std::size_t getSize()
    {
        if constexpr (sizeof...(Ts) > 0)
        {
            return getAttributeSize() + ParentType::getSize();
        }

        return getAttributeSize();
    }

    template<TId attributeId, std::size_t offset = 0>
    static constexpr std::size_t getAttributeOffset()
    {
        if constexpr(T::id == attributeId)
        {
            return offset;
        }
        else if constexpr (sizeof...(Ts) == 1)
        {
            return offset + getAttributeSize();
        }
        else if constexpr(sizeof...(Ts) > 1)
        {
            return ParentType::template getAttributeOffset<attributeId, offset + getAttributeSize()>();
        }
        else
        {
            static_assert(sizeof...(Ts) == 0, "Invalid attribute id");
        }
    }
};

template<AttributeId id, typename T>
struct VertexAttribute: public Attribute<AttributeId, id, T>
{    
};

template<typename TLayout>
struct VertexLayout
{
    static constexpr std::size_t getSize() noexcept
    {
        return TLayout::getSize();
    }

    template<AttributeId Id, typename T>
    static void writeAttribute(const T& attributeValue, Byte* buffer)
    {
        auto offset = TLayout::template getAttributeOffset<Id>();

        std::cout << "Attribute: " << static_cast<unsigned int>(Id) << "; Offset: " << offset << ";" << std::endl;
    }

    template<AttributeId Id>
    static void writePoint(const Point& point, Byte* buffer)
    {
        constexpr auto offset = TLayout::template getAttributeOffset<Id>();

        Byte* cursor = buffer;

        memcpy(cursor + offset, &point.x, sizeof(point.x));
        cursor += sizeof(point.x);
        memcpy(cursor + offset, &point.y, sizeof(point.y));
        cursor += sizeof(point.y);
        memcpy(cursor + offset, &point.z, sizeof(point.z));
        cursor += sizeof(point.z);
    }

    template<AttributeId Id>
    static void writeColor(const Color& color, Byte* buffer)
    {
        constexpr auto offset = TLayout::template getAttributeOffset<Id>();

        Byte* cursor = buffer;

        memcpy(cursor + offset, &color.r, sizeof(color.r));
        cursor += sizeof(color.r);
        memcpy(cursor + offset, &color.g, sizeof(color.g));
        cursor += sizeof(color.g);
        memcpy(cursor + offset, &color.b, sizeof(color.b));
        cursor += sizeof(color.b);
    }

    template<AttributeId Id>
    static void writeTextureCoordinates(const TextureCoordinates& coordinates, Byte* buffer)
    {
        constexpr auto offset = TLayout::template getAttributeOffset<Id>();

        Byte* cursor = buffer;

        memcpy(cursor + offset, &coordinates.u, sizeof(coordinates.u));
        cursor += sizeof(coordinates.u);
        memcpy(cursor + offset, &coordinates.v, sizeof(coordinates.v));
        cursor += sizeof(coordinates.v);
    }

    template<AttributeId Id>
    static void writeSize(const std::size_t& size, Byte* buffer)
    {
        constexpr auto offset = TLayout::template getAttributeOffset<Id>();

        Byte* cursor = buffer;

        memcpy(cursor + offset, &size, sizeof(size));
    }

    static void writePosition(const Point& position, Byte* buffer)
    {
        writePoint<AttributeId::position>(position, buffer);
    }

    static void writeColor(const Color& color, Byte* buffer)
    {
        writeColor<AttributeId::color>(color, buffer);
    }

    static void writeTextureCoordinates(const TextureCoordinates& coordinates, Byte* buffer)
    {
        writeTextureCoordinates<AttributeId::textureCoordinates>(coordinates, buffer);
    }

    static void writeNormal(const Point& normal, Byte* buffer)
    {
        writePoint<AttributeId::normal>(normal, buffer);
    }

    static void writeMeshIndex(const std::size_t& meshIndex, Byte* buffer)
    {
        writeSize<AttributeId::meshIndex>(meshIndex, buffer);
    }
};

TEST_CASE( "Layout test", "[typeMapping]" )
{
    using LayoutMap = Layout<AttributeId,
        VertexAttribute<AttributeId::meshIndex, std::size_t>,
        VertexAttribute<AttributeId::position, Point>, 
        VertexAttribute<AttributeId::color, Color>,
        VertexAttribute<AttributeId::normal, Point>,
        VertexAttribute<AttributeId::textureCoordinates, TextureCoordinates>>;

    using VertexLayout = VertexLayout<LayoutMap>;

    constexpr std::size_t VerticesCount = 1;

    const Point& position = Point{1, 2, 3};
    Point positions[VerticesCount] = { position };

    const Color& color = Color {4, 5, 6};
    Color colors[VerticesCount] = { color };

    const TextureCoordinates& coordinates = {7, 8};
    TextureCoordinates textureCoordinates[VerticesCount] { coordinates };

    const Point& normal = {9, 10, 11};
    Point normals[VerticesCount] = { normal };

    const std::size_t& meshIndex = 12;

    constexpr std::size_t VertexSize = VertexLayout::getSize();
    constexpr std::size_t BufferSize = VertexSize * VerticesCount;
    Byte buffer[VertexLayout::getSize() * VerticesCount];

    Byte* cursor = buffer;
    for(std::size_t i = 0; i < VerticesCount; ++i)
    {
        VertexLayout::writePosition(positions[i], cursor);
        VertexLayout::writeColor(colors[i], cursor);
        VertexLayout::writeTextureCoordinates(textureCoordinates[i], cursor);
        VertexLayout::writeNormal(normals[i], cursor);
        VertexLayout::writeMeshIndex(meshIndex, cursor);

        cursor += VertexSize;
    }

    cursor = buffer;

    const std::size_t& meshIndex2 = *reinterpret_cast<std::size_t*>((Byte*)cursor);
    REQUIRE(meshIndex2 == meshIndex);
    cursor += sizeof(std::size_t);

    const Point& position2 = *reinterpret_cast<Point*>((Byte*)cursor);
    REQUIRE(position2.x == position.x);
    REQUIRE(position2.y == position.y);
    REQUIRE(position2.z == position.z);
    cursor += sizeof(Point);

    const Color& color2 = *reinterpret_cast<Color*>((Byte*)cursor);
    REQUIRE(color2.r == color.r);
    REQUIRE(color2.g == color.g);
    REQUIRE(color2.b == color.b);
    cursor += sizeof(Color);

    const Point& normal2 = *reinterpret_cast<Point*>((Byte*)cursor);
    REQUIRE(normal2.x == normal.x);
    REQUIRE(normal2.y == normal.y);
    REQUIRE(normal2.z == normal.z);
    cursor += sizeof(Point);

    const TextureCoordinates& coordinates2 = *reinterpret_cast<TextureCoordinates*>((Byte*)cursor);
    REQUIRE(coordinates2.u == coordinates.u);
    REQUIRE(coordinates2.v == coordinates.v);
    cursor += sizeof(TextureCoordinates);
}