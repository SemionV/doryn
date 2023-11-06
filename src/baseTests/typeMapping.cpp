#include "dependencies.h"
#include "dataLayout.h"

struct Point
{
    std::size_t x {};
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

using Byte = std::uint8_t;

template<typename TLayout>
struct VertexSerializer
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

template<AttributeId id, typename T>
struct VertexAttribute: public dory::Attribute<AttributeId, id, T>
{    
};

TEST_CASE( "Layout test", "[typeMapping]" )
{
    using LayoutMap = dory::Layout<AttributeId,
        VertexAttribute<AttributeId::meshIndex, std::size_t>,
        VertexAttribute<AttributeId::position, Point>, 
        VertexAttribute<AttributeId::color, Color>,
        VertexAttribute<AttributeId::normal, Point>,
        VertexAttribute<AttributeId::textureCoordinates, TextureCoordinates>>;

    using VertexSerializer = VertexSerializer<LayoutMap>;

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

    constexpr std::size_t VertexSize = VertexSerializer::getSize();
    constexpr std::size_t BufferSize = VertexSize * VerticesCount;
    Byte buffer[VertexSerializer::getSize() * VerticesCount];

    Byte* cursor = buffer;
    for(std::size_t i = 0; i < VerticesCount; ++i)
    {
        VertexSerializer::writePosition(positions[i], cursor);
        VertexSerializer::writeColor(colors[i], cursor);
        VertexSerializer::writeTextureCoordinates(textureCoordinates[i], cursor);
        VertexSerializer::writeNormal(normals[i], cursor);
        VertexSerializer::writeMeshIndex(meshIndex, cursor);

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

//-------------------------------------------------------------------------------------------------------------------------

template <class T>
struct MemberPointerType;

template <class C, class T>
struct MemberPointerType<T C::*> 
{ 
    using type = T;
    using classType = C;

    static constexpr std::size_t getSize()
    {
        return sizeof(T);
    }
};

template <class T>
struct MemberType : MemberPointerType<typename std::remove_cv<T>::type> 
{    
};

template <class T>
using member_type_t = typename MemberType<T>::type;

template<typename TMemberPointer, TMemberPointer memberPointer>
struct MemberBinding
{
    static constexpr std::size_t getSize()
    {
        return MemberType<TMemberPointer>::getSize();
    }

    static constexpr TMemberPointer getMemberPointer()
    {
        return memberPointer;
    }
};

TEST_CASE( "Reflection test", "[typeMapping]" )
{
    constexpr auto xPointer = &Point::x;
    using PointXBinding = MemberBinding<decltype(xPointer), xPointer>;
    
    std::cout << "Point::x size: " << PointXBinding::getSize() << std::endl;

    Point p {456, 2, 3};

    std::cout << "p.x value: " << p.*PointXBinding::getMemberPointer() << std::endl;
}