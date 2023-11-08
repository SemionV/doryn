#include "dependencies.h"
#include "dataLayout.h"

struct TextureCoordinates
{
    int u {};
    int v {};
};

struct Color
{
    int r {};
    int g {};
    int b {};
    TextureCoordinates coords {};
};

struct Point
{
    std::size_t x {};
    int y {};
    int z {};
    Color color {};
};

struct serializable : refl::attr::usage::field, refl::attr::usage::function
{
};

template<typename T>
struct VertexAttributeType: public T
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
    REFL_FIELD(coords, serializable())
REFL_END

REFL_TYPE(Point)
    REFL_FIELD(x, serializable())
    REFL_FIELD(y, serializable())
    REFL_FIELD(z, serializable())
    REFL_FIELD(color, serializable())
REFL_END

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

using Byte = std::uint8_t;

template<typename TLayout, typename TId>
struct VertexSerializer
{
    static constexpr std::size_t getSize() noexcept
    {
        return TLayout::getSize();
    }

    template<typename T>
    static std::size_t writeTrivialValue(const T attributeValue, Byte* buffer)
    {
        auto size = sizeof(T);
        memcpy(buffer, &attributeValue, size);
        return size;
    }

    template<typename T, typename TDescriptor>
    static std::size_t writeComplexValue(const T& attributeValue, Byte* buffer, TDescriptor typeDescriptor)
    {
        std::size_t offset = {};

        for_each(typeDescriptor.members, [&](auto member)
        {
            if constexpr (is_readable(member))
            {
                using memberType = decltype(member);

                if constexpr (std::is_trivial_v<typename memberType::value_type>)
                {
                    offset += writeTrivialValue(member(attributeValue), buffer + offset);
                }
                else
                {
                    constexpr refl::descriptor::type_descriptor<typename memberType::value_type> typeDescriptor{};
                    offset += writeComplexValue(member(attributeValue), buffer + offset, typeDescriptor);
                }
            }
        });

        return offset;
    }

    template<TId Id, typename T>
    static std::size_t writeAttribute(const T& attributeValue, Byte* buffer)
    {
        std::size_t size = {};

        auto offset = TLayout::template getAttributeOffset<Id>();

        if constexpr (std::is_trivial_v<T>)
        {
            size += writeTrivialValue(attributeValue, buffer + offset);
        }
        else
        {
            auto typeDescriptor = TLayout::template getAttributeTypeDescriptor<Id>();
            size += writeComplexValue(attributeValue, buffer + offset, typeDescriptor);
        }

        std::cout << "Attribute: " << static_cast<unsigned int>(Id) << "; Offset: " << offset << ";" << " Size: " << size << std::endl;

        return size;
    }
};

enum class AttributeId
{
    meshId,
    position,
    color,
    textureCoordinates,
    normal
};

template<AttributeId id, typename T>
struct VertexAttribute: public dory::Attribute<AttributeId, id, T>
{    
};

TEST_CASE( "Layout test", "[typeMapping]" )
{
    using LayoutMap = dory::Layout<AttributeId,
        VertexAttribute<AttributeId::meshId, std::size_t>,
        VertexAttribute<AttributeId::position, VertexAttributeType<Point>>, 
        VertexAttribute<AttributeId::color, VertexAttributeType<Color>>,
        VertexAttribute<AttributeId::normal, VertexAttributeType<Point>>,
        VertexAttribute<AttributeId::textureCoordinates, VertexAttributeType<TextureCoordinates>>>;

    using VertexSerializer = VertexSerializer<LayoutMap, AttributeId>;

    constexpr std::size_t VerticesCount = 1;

    const Point& position = Point{1, 2, 3};
    Point positions[VerticesCount] = { position };

    const Color& color = Color {4, 5, 6};
    Color colors[VerticesCount] = { color };

    const TextureCoordinates& coordinates = {7, 8};
    TextureCoordinates textureCoordinates[VerticesCount] { coordinates };

    const Point& normal = {9, 10, 11};
    Point normals[VerticesCount] = { normal };

    const std::size_t& meshId = 12;

    constexpr std::size_t VertexSize = VertexSerializer::getSize();
    constexpr std::size_t BufferSize = VertexSize * VerticesCount;
    Byte buffer[VertexSerializer::getSize() * VerticesCount];

    Byte* cursor = buffer;
    for(std::size_t i = 0; i < VerticesCount; ++i)
    {
        VertexSerializer::writeAttribute<AttributeId::meshId>(meshId, cursor);
        VertexSerializer::writeAttribute<AttributeId::position>(positions[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::color>(colors[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::normal>(normals[i], cursor);
        VertexSerializer::writeAttribute<AttributeId::textureCoordinates>(textureCoordinates[i], cursor);

        cursor += VertexSize;
    }

    cursor = buffer;

    const std::size_t& meshId2 = *reinterpret_cast<std::size_t*>((Byte*)cursor);
    REQUIRE(meshId2 == meshId);
    cursor += sizeof(std::size_t);

    const Point& position2 = *reinterpret_cast<Point*>((Byte*)cursor);
    REQUIRE(position2.x == position.x);
    REQUIRE(position2.y == position.y);
    REQUIRE(position2.z == position.z);
    cursor += LayoutMap::getAttributeSize<AttributeId::position>();

    const Color& color2 = *reinterpret_cast<Color*>((Byte*)cursor);
    REQUIRE(color2.r == color.r);
    REQUIRE(color2.g == color.g);
    REQUIRE(color2.b == color.b);
    cursor += LayoutMap::getAttributeSize<AttributeId::color>();

    const Point& normal2 = *reinterpret_cast<Point*>((Byte*)cursor);
    REQUIRE(normal2.x == normal.x);
    REQUIRE(normal2.y == normal.y);
    REQUIRE(normal2.z == normal.z);
    cursor += LayoutMap::getAttributeSize<AttributeId::normal>();

    const TextureCoordinates& coordinates2 = *reinterpret_cast<TextureCoordinates*>((Byte*)cursor);
    REQUIRE(coordinates2.u == coordinates.u);
    REQUIRE(coordinates2.v == coordinates.v);
    cursor += LayoutMap::getAttributeSize<AttributeId::textureCoordinates>();
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
struct MemberMapping
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

TEST_CASE( "Member Reflection test", "[typeMapping]" )
{
    constexpr auto xPointer = &Point::x;
    using PointXMapping = MemberMapping<decltype(xPointer), xPointer>;

    REQUIRE(PointXMapping::getSize() == sizeof(Point::x));

    Point p {456, 2, 3};

    REQUIRE(p.*PointXMapping::getMemberPointer() == 456);
}

template <typename C, typename... TMemberMappings>
struct ClassMapping;

template <typename C>
struct ClassMapping<C>
{
    static constexpr std::size_t getSize()
    {
        return 0;
    }

    static constexpr std::size_t getCount()
    {
        return 0;
    }
};

template<typename C, typename TMemberMapping, typename... TMemberMappings>
struct ClassMapping<C, TMemberMapping, TMemberMappings...>: ClassMapping<C, TMemberMappings...>
{
    using ParentType = ClassMapping<C, TMemberMappings...>;

    static constexpr std::size_t getSize()
    {
        auto size = TMemberMapping::getSize();
        
        if constexpr (sizeof...(TMemberMappings) > 0)
        {
            size += ParentType::getSize();
        }

        return size;
    }

    static constexpr std::size_t getCount()
    {
        return sizeof...(TMemberMappings) + 1;
    }
};

template<typename C>
struct ClassMappingType: ClassMapping<C>
{
};

template<>
struct ClassMappingType<Point>: ClassMapping<Point, 
    MemberMapping<decltype(&Point::x), &Point::x>, 
    MemberMapping<decltype(&Point::y), &Point::y>, 
    MemberMapping<decltype(&Point::z), &Point::z>>
{
};

/*#define EMPTY_CLASS_NAME
#define CURRENT_CLASS_TYPE Point
//#define SET_CURRENT_CLASS_TYPE(ClassName)(CURRENT_CLASS_TYPE=(ClassName))

#define BEGIN_CLASS_MAP(ClassType)\
template<>\
struct ClassMappingType<CURRENT_CLASS_TYPE>: ClassMapping<CURRENT_CLASS_TYPE

#define MAP_MEMBER(MemberName)\
,MemberMapping<decltype(&CURRENT_CLASS_TYPE::MemberName), &CURRENT_CLASS_TYPE::MemberName>

#define END_CLASS_MAP >{};

BEGIN_CLASS_MAP(Point)
    MAP_MEMBER(x)
    MAP_MEMBER(y)
    MAP_MEMBER(z)
END_CLASS_MAP*/

TEST_CASE( "Class Reflection test", "[.][typeMapping]" )
{
    std::cout << "Point Mapping size: " << ClassMappingType<Point>::getSize() << std::endl;
    std::cout << "Point Mapping count: " << ClassMappingType<Point>::getCount() << std::endl;
    std::cout << "Color Mapping size: " << ClassMappingType<Color>::getSize() << std::endl;
    std::cout << "Color Mapping count: " << ClassMappingType<Color>::getCount() << std::endl;
}

//-------------------------------------------------------------------------------------------------------------------------

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

TEST_CASE( "refl-cpp serialization test", "[typeMapping]" )
{
    Point point {
        1, 
        2, 
        3, 
        Color 
        {
            4, 
            5, 
            6,
            TextureCoordinates
            {
                7, 
                8
            }
        }};

    serialize(std::cout, point);
}