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

template<typename T>
class MemberMapping
{
    public:
        virtual ~MemberMapping() = default;

        virtual std::size_t getSize() = 0;
        virtual void writeTo(const T& object,  void* buffer) = 0;
};

template<typename T, typename M, M T::*m>
class MemberPointerMapping: public MemberMapping<T>
{
    public:
        std::size_t getSize() override
        {
            return sizeof(M);
        }

        void writeTo(const T& object, void* buffer) override
        {
            *static_cast<M*>(buffer) = object.*m;
        }
};

class TypeMapping
{
    MemberPointerMapping<Point, decltype(Point::x), &Point::x> pointXMapping;
};

void testBody()
{
    MemberPointerMapping<Point, decltype(Point::x), &Point::x> pointXMapping;

    Point point {3};

    REQUIRE(pointXMapping.getSize() == sizeof(int));

    Point buffer;

    pointXMapping.writeTo(point, &buffer.x);
    REQUIRE(buffer.x == 3);
}

TEST_CASE( "Type mapping test", "[.][typeMapping]" ) 
{
    testBody();
}

using AttributeIndex = unsigned short;
using Byte = std::uint8_t;

template<typename TId, TId Id, typename T>
struct Attribute
{
    static constexpr TId id = Id;
    using AttributeType = T;
};

template <class... Ts>
struct Layout;

template<>
struct Layout<>
{
};

template<typename T, typename... Ts>
struct Layout<T, Ts...>: public Layout<Ts...>
{
    static constexpr std::size_t getAttributeSize()
    {
        return sizeof(typename T::AttributeType);
    }

    static constexpr std::size_t getSize()
    {
        if constexpr (sizeof...(Ts) > 0)
        {
            return getAttributeSize() + Layout<Ts...>::getSize();
        }

        return getAttributeSize();
    }

    template<typename TId, TId attributeId, std::size_t offset = 0>
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
            return Layout<Ts...>::template getAttributeOffset<TId, attributeId, offset + getAttributeSize()>();
        }
        else
        {
            static_assert(sizeof...(Ts) == 0, "Invalid attribute id");
        }
    }
};

enum class AttributeId
{
    position,
    color,
    textureCoordinates,
    normal
};

struct VertexLayout
{
    using LayoutMap = Layout<Attribute<AttributeId, AttributeId::position, Point>, 
        Attribute<AttributeId, AttributeId::color, Color>, 
        Attribute<AttributeId, AttributeId::textureCoordinates, TextureCoordinates>, 
        Attribute<AttributeId, AttributeId::normal, Point>>;

    static constexpr std::size_t getSize() noexcept
    {
        return LayoutMap::getSize();
    }

    template<AttributeId Id, typename T>
    static void writeAttribute(const T& attribute, Byte* buffer)
    {
        auto offset = LayoutMap::getAttributeOffset<AttributeId, Id>();

        std::cout << "Attribute: " << static_cast<unsigned int>(Id) << "; Offset: " << offset << ";" << std::endl;
    }

    static void writePosition(const Point& position, Byte* buffer)
    {
        writeAttribute<AttributeId::position>(position, buffer);
    }

    static void writeColor(const Color& color, Byte* buffer)
    {
        writeAttribute<AttributeId::color>(color, buffer);
    }

    static void writeTextureCoordinates(const TextureCoordinates& textureCoordinates, Byte* buffer)
    {
        writeAttribute<AttributeId::textureCoordinates>(textureCoordinates, buffer);
    }

    static void writeNormal(const Point& normal, Byte* buffer)
    {
        writeAttribute<AttributeId::normal>(normal, buffer);
    }
};

void writeVertexPositions(Point* positions, Color* colors, std::size_t count, Byte* buffer)
{
    Byte* cursor = buffer;
    for(std::size_t i = 0; i < count; ++i)
    {
        const Point& position = positions[i];
        const Color& color = colors[i];
        
        memcpy(cursor, &position.x, sizeof(position.x));
        cursor += sizeof(position.x);
        memcpy(cursor, &position.y, sizeof(position.y));
        cursor += sizeof(position.y);
        memcpy(cursor, &position.z, sizeof(position.z));
        cursor += sizeof(position.z);

        memcpy(cursor, &color.r, sizeof(color.r));
        cursor += sizeof(color.r);
        memcpy(cursor, &color.g, sizeof(color.g));
        cursor += sizeof(color.g);
        memcpy(cursor, &color.b, sizeof(color.b));
        cursor += sizeof(color.b);
    }
}

TEST_CASE( "Layout test", "[typeMapping]" )
{
    std::tuple<Point, Color, TextureCoordinates, Point> t;

    const Point& point = std::get<0>(t);

    std::cout << "Vertex check size: " << sizeof(Point) + sizeof(Color) + sizeof(TextureCoordinates) + sizeof(Point) << std::endl;
    std::cout << "Vertex layout size: " << VertexLayout::getSize() << std::endl;

    constexpr std::size_t VerticesCount = 100;

    Point positions[VerticesCount];
    Color colors[VerticesCount];
    TextureCoordinates textureCoordinates[VerticesCount];
    Point normals[VerticesCount];

    constexpr std::size_t BufferSize = VertexLayout::getSize() * VerticesCount;
    std::cout << "Vertex Buffer size: " << BufferSize << std::endl;

    Byte buffer[BufferSize];

    for(std::size_t i = 0; i < 1; ++i)
    {
        VertexLayout::writePosition(positions[i], buffer);
        VertexLayout::writeColor(colors[i], buffer);
        VertexLayout::writeTextureCoordinates(textureCoordinates[i], buffer);
        VertexLayout::writeNormal(normals[i], buffer);
    }
}