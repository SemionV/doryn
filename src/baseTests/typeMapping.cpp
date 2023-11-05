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
    normal
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

struct VertexLayout
{
    using LayoutMap = Layout<AttributeId,
        VertexAttribute<AttributeId::position, Point>, 
        VertexAttribute<AttributeId::color, Color>, 
        VertexAttribute<AttributeId::textureCoordinates, TextureCoordinates>,
        VertexAttribute<AttributeId::normal, Point>>;

    static constexpr std::size_t getSize() noexcept
    {
        return LayoutMap::getSize();
    }

    template<AttributeId Id, typename T>
    static void writeAttribute(const T& attribute, Byte* buffer)
    {
        auto offset = LayoutMap::getAttributeOffset<Id>();

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