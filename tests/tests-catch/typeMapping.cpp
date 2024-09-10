#include <catch2/catch_test_macros.hpp>
#include <refl.hpp>

#include <dory/serialization/structures/dataLayout.h>
#include <dory/serialization/binaryLayoutSerializer.h>
#include <dory/serialization/jsonPrinter.h>

struct TextureCoordinates
{
    int u {};
    int v {};
};

struct Color
{
    float r {};
    float g {};
    float b {};
};

struct Point
{
    float x {};
    float y {};
    float z {};
};

struct DoublePoint
{
    Point p1;
    Point p2;
};

struct serializable : refl::attr::usage::field, refl::attr::usage::function
{
};

REFL_TYPE(TextureCoordinates)
    REFL_FIELD(u, serializable())
    REFL_FIELD(v, serializable())
REFL_END

REFL_TYPE(Color)
    REFL_FIELD(r, serializable())
    REFL_FIELD(g, serializable())
    REFL_FIELD(b, serializable())
REFL_END

REFL_TYPE(Point)
    REFL_FIELD(x, serializable())
    REFL_FIELD(y, serializable())
    REFL_FIELD(z, serializable())
REFL_END

REFL_TYPE(DoublePoint)
    REFL_FIELD(p1)
    REFL_FIELD(p2)
REFL_END

template<typename T>
struct VertexAttributeType: public T
{
};

REFL_TYPE(VertexAttributeType<Point>)
    REFL_FIELD(x)
    REFL_FIELD(y)
    REFL_FIELD(z)
REFL_END

REFL_TYPE(VertexAttributeType<Color>)
    REFL_FIELD(r)
    REFL_FIELD(g)
    REFL_FIELD(b)
REFL_END

REFL_TYPE(VertexAttributeType<TextureCoordinates>)
    REFL_FIELD(u)
    REFL_FIELD(v)
REFL_END

REFL_TYPE(VertexAttributeType<DoublePoint>)
    REFL_FIELD(p1)
    REFL_FIELD(p2)
REFL_END

enum class AttributeId
{
    meshId,
    position,
    color,
    textureCoordinates,
    normal,
    doublePoint
};

template<auto Id, typename T>
using LayoutAttribute = dory::serialization::Attribute<Id, T>;

using LayoutMap = dory::serialization::Layout<LayoutAttribute<AttributeId::meshId, std::size_t>,
        LayoutAttribute<AttributeId::position, VertexAttributeType<Point>>,
        LayoutAttribute<AttributeId::color, VertexAttributeType<Color>>,
        LayoutAttribute<AttributeId::normal, VertexAttributeType<Point>>,
        LayoutAttribute<AttributeId::textureCoordinates, VertexAttributeType<TextureCoordinates>>,
        LayoutAttribute<AttributeId::doublePoint, VertexAttributeType<DoublePoint>>>;

TEST_CASE( "Layout typeMap test", "[typeMapping]" )
{
    using VertexSerializer = dory::serialization::BinaryLayoutSerializer<LayoutMap>;

    constexpr std::size_t VerticesCount = 2;

    Point positions[VerticesCount] = { Point{1, 2, 3},  Point{4, 5, 6}};
    Color colors[VerticesCount] = { Color {4, 5, 6}, Color {7, 8, 9} };
    TextureCoordinates textureCoordinates[VerticesCount] = { TextureCoordinates{7, 8}, TextureCoordinates{9, 10} };
    Point normals[VerticesCount] = { Point{9, 10, 11}, Point{12, 13, 14} };
    DoublePoint doublePoints[VerticesCount] = { DoublePoint{ Point{9, 10, 11}, Point{12, 13, 14}}, DoublePoint{ Point{9, 10, 11}, Point{12, 13, 14}} };
    const std::size_t& meshId = 12;

    constexpr std::size_t VertexSize = dory::serialization::LayoutSizeV<LayoutMap>;
    dory::Byte buffer[VertexSize * VerticesCount];

    dory::Byte* cursor = buffer;
    for(std::size_t i = 0; i < VerticesCount; ++i)
    {
        VertexSerializer::writeAttribute<AttributeId::meshId>(meshId, cursor);
        VertexSerializer::writeAttribute<AttributeId::position>(positions[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::color>(colors[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::normal>(normals[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::textureCoordinates>(textureCoordinates[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::doublePoint>(doublePoints[i], cursor);

        cursor += VertexSize;
    }

    cursor = buffer;
    Point positions2[VerticesCount];
    Color colors2[VerticesCount];
    Point normals2[VerticesCount];
    TextureCoordinates textureCoordinates2[VerticesCount];
    DoublePoint doublePoints2[VerticesCount];
    std::size_t meshId2 {};

    for(std::size_t i = 0; i < VerticesCount; ++i)
    {
        VertexSerializer::readAttribute<AttributeId::meshId>(meshId2, cursor);
        VertexSerializer::readAttribute<AttributeId::position>(positions2[i], cursor);
        VertexSerializer::readAttribute<AttributeId::color>(colors2[i], cursor);
        VertexSerializer::readAttribute<AttributeId::normal>(normals2[i], cursor);
        VertexSerializer::readAttribute<AttributeId::textureCoordinates>(textureCoordinates2[i], cursor);
        VertexSerializer::readAttribute<AttributeId::doublePoint>(doublePoints2[i], cursor);

        REQUIRE(meshId2 == meshId);

        REQUIRE(positions2[i].x == positions[i].x);
        REQUIRE(positions2[i].y == positions[i].y);
        REQUIRE(positions2[i].z == positions[i].z);

        REQUIRE(colors2[i].r == colors[i].r);
        REQUIRE(colors2[i].g == colors[i].g);
        REQUIRE(colors2[i].b == colors[i].b);

        REQUIRE(normals2[i].x == normals[i].x);
        REQUIRE(normals2[i].y == normals[i].y);
        REQUIRE(normals2[i].z == normals[i].z);

        REQUIRE(textureCoordinates2[i].u == textureCoordinates[i].u);
        REQUIRE(textureCoordinates2[i].v == textureCoordinates[i].v);

        REQUIRE(doublePoints2[i].p1.x == doublePoints[i].p1.x);
        REQUIRE(doublePoints2[i].p1.y == doublePoints[i].p1.y);
        REQUIRE(doublePoints2[i].p1.z == doublePoints[i].p1.z);
        REQUIRE(doublePoints2[i].p2.x == doublePoints[i].p2.x);
        REQUIRE(doublePoints2[i].p2.y == doublePoints[i].p2.y);
        REQUIRE(doublePoints2[i].p2.z == doublePoints[i].p2.z);

        cursor += VertexSize;
    }
}

template<typename LayoutMap, typename T, typename TMembers, AttributeId attributeId, std::size_t membersCount, std::size_t offset>
void testAttributeDescriptor(std::size_t customSize = 0)
{
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeTypeT<attributeId, LayoutMap>, T>);
    REQUIRE(std::is_same_v<dory::serialization::LayoutAttributeFieldTypeT<attributeId, LayoutMap>, TMembers>);

    const auto attributeSize = dory::serialization::LayoutAttributeSizeV<attributeId, LayoutMap>;
    if constexpr (std::is_same_v<TMembers, void>)
    {
        REQUIRE(attributeSize == sizeof(T));
    }
    else
    {
        REQUIRE(attributeSize == (customSize != 0 ? customSize : sizeof(TMembers) * membersCount));
    }
    REQUIRE(dory::serialization::LayoutAttributeOffsetV<attributeId, LayoutMap> == offset);
    REQUIRE(dory::serialization::LayoutAttributeFieldCountV<attributeId, LayoutMap> == membersCount);
}

TEST_CASE( "Layout attributes check", "[typeMapping]" )
{
    testAttributeDescriptor<LayoutMap, std::size_t, std::size_t, AttributeId::meshId, 1, 0>();
    testAttributeDescriptor<LayoutMap, VertexAttributeType<Point>, float, AttributeId::position, 3, 8>();
    testAttributeDescriptor<LayoutMap, VertexAttributeType<Color>, float, AttributeId::color, 3, 20>();
    testAttributeDescriptor<LayoutMap, VertexAttributeType<Point>, float, AttributeId::normal, 3, 32>();
    testAttributeDescriptor<LayoutMap, VertexAttributeType<TextureCoordinates>, int, AttributeId::textureCoordinates, 2, 44>();
    testAttributeDescriptor<LayoutMap, VertexAttributeType<DoublePoint>, float, AttributeId::doublePoint, 6, 52>();
}

class PointToVertexPointConverter
{
    public:
        static VertexAttributeType<Point> convert(Point point)
        {
            return VertexAttributeType<Point>{ point.x, point.y, point.z + 1};
        }
};

class VertexPointToPointConverter
{
    public:
        static Point convert(VertexAttributeType<Point> point)
        {
            return Point{ point.x, point.y, point.z - 1};
        }
};

template<typename TRight, typename TConverter, typename TLeft>
TRight convert(TLeft value)
{
    return TConverter::convert(value);
}

using TypeMap = dory::serialization::TypeMap<dory::serialization::TypeAssigment<dory::serialization::TypePair<Point, VertexAttributeType<Point>>, PointToVertexPointConverter, VertexPointToPointConverter>,
    dory::serialization::TypeAssigment<dory::serialization::TypePair<Color, VertexAttributeType<Color>>>,
    dory::serialization::TypeAssigment<dory::serialization::TypePair<TextureCoordinates, VertexAttributeType<TextureCoordinates>>>>;

TEST_CASE( "Type Map", "[typeMapping]" )
{
    REQUIRE(std::is_same_v<dory::serialization::MappedTypeT<Point, TypeMap>, VertexAttributeType<Point>>);
    REQUIRE(std::is_same_v<dory::serialization::MappedTypeT<Color, TypeMap>, VertexAttributeType<Color>>);
    REQUIRE(std::is_same_v<dory::serialization::MappedTypeT<TextureCoordinates, TypeMap>, VertexAttributeType<TextureCoordinates>>);
    REQUIRE(std::is_same_v<dory::serialization::MappedTypeT<DoublePoint, TypeMap>, DoublePoint>);

    using DestinationType = dory::serialization::MappedTypeT<Point, TypeMap>;
    using ForwardConverterType = typename dory::serialization::MappedType<Point, TypeMap>::ForwardConverterType;
    using BackwardConverterType = typename dory::serialization::MappedType<Point, TypeMap>::BackwardConverterType;

    Point point = {1, 2, 3};
    auto vertexPoint = convert<DestinationType, ForwardConverterType>(point);
    REQUIRE(vertexPoint.x == 1);
    REQUIRE(vertexPoint.y == 2);
    REQUIRE(vertexPoint.z == 4);

    auto pointBack = convert<Point, BackwardConverterType>(vertexPoint);
    REQUIRE(pointBack.x == point.x);
    REQUIRE(pointBack.y == point.y);
    REQUIRE(pointBack.z == point.z);
}

TEST_CASE( "Print reflected object", "[.][typeMapping]" )
{
    VertexAttributeType<DoublePoint> dPoint { Point{9, 10, 11}, Point{12, 13, 14}};
    dory::serialization::ObjectPrinter::print(dPoint);
    dory::serialization::ObjectPrinter::print(VertexAttributeType<DoublePoint>{Point{9, 10, 11}, Point{12, 13, 14}});

    dory::serialization::ObjectPrinter::print(999);

    dory::serialization::ObjectPrinter::print(std::array<int, 5>{1, 2, 3, 4, 5});
}

/*namespace refl_impl::metadata 
{ 
    template<> struct type_info__<Point> 
    { 
        typedef Point type; 
        
        static constexpr auto attributes{ ::refl::detail::make_attributes<::refl::attr::usage:: type>() }; 
        static constexpr auto name{ ::refl::util::make_const_string("Point") }; 
        static constexpr size_t member_index_offset = 72 + 1; 
        
        template <size_t, typename = void> 
        struct member {};
        
        template<typename Unused__> 
        struct member<73 - member_index_offset, Unused__> 
        { 
            typedef ::refl::member::field member_type; 
            static constexpr auto name{ ::refl::util::make_const_string("x") }; 
            static constexpr auto attributes{ ::refl::detail::make_attributes<::refl::attr::usage:: field>(serializable()) }; 
            
            public: 
                typedef decltype(type::x) value_type; 
                static constexpr auto pointer{ &type::x }; 
                
                template <typename Proxy> 
                struct remap 
                { 
                    template <typename... Args> 
                    decltype(auto) x(Args&&... args) 
                    { 
                        return Proxy::invoke_impl(static_cast<Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                    
                    template <typename... Args> 
                    decltype(auto) x(Args&&... args) const 
                    { 
                        return Proxy::invoke_impl(static_cast<const Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                };
        };
        
        template<typename Unused__> 
        struct member<74 - member_index_offset, Unused__> 
        { 
            typedef ::refl::member::field member_type; 
            static constexpr auto name{ ::refl::util::make_const_string("y") }; 
            static constexpr auto attributes{ ::refl::detail::make_attributes<::refl::attr::usage:: field>(serializable()) };

            public: 
                typedef decltype(type::y) value_type; 
                static constexpr auto pointer{ &type::y }; 
                
                template <typename Proxy> 
                struct remap 
                { 
                    template <typename... Args> 
                    decltype(auto) y(Args&&... args) 
                    { 
                        return Proxy::invoke_impl(static_cast<Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                    
                    template <typename... Args> 
                    decltype(auto) y(Args&&... args) const 
                    { 
                        return Proxy::invoke_impl(static_cast<const Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                }; 
        };
        
        template<typename Unused__> 
        struct member<75 - member_index_offset, Unused__> 
        { 
            typedef ::refl::member::field member_type; 
            static constexpr auto name{ ::refl::util::make_const_string("z") };            
            static constexpr auto attributes{ ::refl::detail::make_attributes<::refl::attr::usage:: field>(serializable()) }; 
            
            public: 
                typedef decltype(type::z) value_type;
                static constexpr auto pointer{ &type::z }; 
                
                template <typename Proxy> struct remap 
                { 
                    template <typename... Args> 
                    decltype(auto) z(Args&&... args) 
                    { 
                        return Proxy::invoke_impl(static_cast<Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                    
                    template <typename... Args> 
                    decltype(auto) z(Args&&... args) const 
                    { 
                        return Proxy::invoke_impl(static_cast<const Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                }; 
        };

        template<typename Unused__> 
        struct member<76 - member_index_offset, Unused__> 
        { 
            typedef ::refl::member::field member_type; 
            static constexpr auto name{ ::refl::util::make_const_string("color") };
            static constexpr auto attributes{ ::refl::detail::make_attributes<::refl::attr::usage:: field>(serializable()) }; 
            
            public: 
                typedef decltype(type::color) value_type; 
                static constexpr auto pointer{ &type::color }; 
                
                template <typename Proxy> 
                struct remap 
                { 
                    template <typename... Args> 
                    decltype(auto) color(Args&&... args) 
                    { 
                        return Proxy::invoke_impl(static_cast<Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                    
                    template <typename... Args> 
                    decltype(auto) color(Args&&... args) const 
                    { 
                        return Proxy::invoke_impl(static_cast<const Proxy&>(*this), ::std::forward<Args>(args)...); 
                    } 
                }; 
        };
        
        static constexpr size_t member_count{ 77 - member_index_offset }; 
    }; 
}*/

template <typename T>
void serialize(std::ostream& os, T&& value, const std::string& indent = "")
{
    refl::descriptor::type_descriptor<std::remove_reference_t<T>> typeDescriptor{};

    // iterate over the members of T
    for_each(typeDescriptor.members, [&](auto member)
    {
        // is_readable checks if the member is a non-const field
        // or a 0-arg const-qualified function marked with property attribute
        if constexpr (is_readable(member) && refl::descriptor::has_attribute<serializable>(member))
        {
            using memberType = decltype(member);

            os << indent;

            // get_display_name prefers the friendly_name of the property over the function name
            os << get_display_name(member) << " = ";
            // member(value) returns a reference to the field or calls the property accessor
            if constexpr (std::is_trivial_v<typename memberType::value_type>)
            {
                os << member(value) << ";" << std::endl;
            }
            else
            {
                os << std::endl;
                serialize(os, member(value), indent + "    ");
            }
        }
    });
}

TEST_CASE( "refl-cpp typeMap test", "[.][typeMapping]" )
{
    Point point {1, 2, 3};

    serialize(std::cout, point);
}