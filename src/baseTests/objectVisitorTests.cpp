#include "baseTests/dependencies.h"
#include "templates/objectVisitor.h"

namespace dory::typeMap
{
    struct Point
    {
        float x {0};
        float y {0};
        float z {0};
    };

    struct Color
    {
        float r {0};
        float g {0};
        float b {0};
        float a {0};
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
}

REFL_TYPE(dory::typeMap::Point)
        REFL_FIELD(x)
        REFL_FIELD(y)
        REFL_FIELD(z)
REFL_END

REFL_TYPE(dory::typeMap::Color)
        REFL_FIELD(r)
        REFL_FIELD(g)
        REFL_FIELD(b)
        REFL_FIELD(a)
REFL_END

REFL_TYPE(dory::typeMap::Axises)
        REFL_FIELD(i)
        REFL_FIELD(j)
        REFL_FIELD(k)
REFL_END

REFL_TYPE(dory::typeMap::AffineTransformation)
        REFL_FIELD(translation)
        REFL_FIELD(rotation)
        REFL_FIELD(axises)
REFL_END

namespace dory::typeMap
{
    struct ValueNode;

    using ObjectRepresentation = std::map<std::string, std::shared_ptr<ValueNode>>;
    using CollectionRepresentation = std::vector<std::shared_ptr<ValueNode>>;
    using ValueType = std::variant<std::size_t, float, ObjectRepresentation, CollectionRepresentation>;

    struct ValueNode
    {
        ValueType value;
        std::shared_ptr<ValueNode> parent;

        explicit ValueNode(std::shared_ptr<ValueNode> parent):
            parent(std::move(parent))
        {
        }
    };

    struct VisitorContext
    {
        std::shared_ptr<ValueNode> currentValueNode;

        explicit VisitorContext(std::shared_ptr<ValueNode> rootValueNode):
                currentValueNode(std::move(rootValueNode))
        {
        }
    };

    struct ValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T&& value, TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode->value = value;
        }
    };

    struct BeginObjectPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode->value = ObjectRepresentation();
        }
    };

    struct EndObjectPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
        }
    };

    struct BeginMemberPolicy
    {
        template<typename TContext>
        inline static void process(const std::string& memberName, const std::size_t i, TContext& context)
        {
            assert(std::holds_alternative<ObjectRepresentation>(context.currentValueNode->value));
            auto valueNode = std::make_shared<ValueNode>(context.currentValueNode);
            std::get<ObjectRepresentation>(context.currentValueNode->value).emplace(memberName, valueNode);
            context.currentValueNode = valueNode;
        }
    };

    struct EndMemberPolicy
    {
        template<typename TContext>
        inline static void process(const bool lastMember, TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode = context.currentValueNode->parent;
            assert(std::holds_alternative<ObjectRepresentation>(context.currentValueNode->value));
        }
    };

    struct BeginCollectionPolicy
    {
        template<typename T, auto N, typename TContext>
        inline static void process(TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode->value = CollectionRepresentation();
        }
    };

    struct EndCollectionPolicy
    {
        template<typename TContext>
        inline static void process(TContext& context)
        {
        }
    };

    struct BeginCollectionItemPolicy
    {
        template<typename TContext>
        inline static void process(const std::size_t i, TContext& context)
        {
            assert(std::holds_alternative<CollectionRepresentation>(context.currentValueNode->value));
            auto valueNode = std::make_shared<ValueNode>(context.currentValueNode);
            context.currentValueNode->value.emplace_back(valueNode);
            context.currentValueNode = valueNode;
        }
    };

    struct EndCollectionItemPolicy
    {
        template<typename TContext>
        inline static void process(const bool lastItem, TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode = context.currentValueNode->parent;
            context.currentValueNode = context.currentValueNode->parent;
            assert(std::holds_alternative<CollectionRepresentation>(context.currentValueNode->value));
        }
    };

    struct VisitorPolicies
    {
        using ValuePolicy = ValuePolicy;
        using BeginObjectPolicy = BeginObjectPolicy;
        using EndObjectPolicy = EndObjectPolicy;
        using BeginMemberPolicy = BeginMemberPolicy;
        using EndMemberPolicy = EndMemberPolicy;
        using BeginCollectionPolicy = BeginCollectionPolicy;
        using EndCollectionPolicy = EndCollectionPolicy;
        using BeginCollectionItemPolicy = BeginCollectionItemPolicy;
        using EndCollectionItemPolicy = EndCollectionItemPolicy;
    };

    template<typename T>
    struct ValueExpected
    {
        using Type = T;
        const T value;
        explicit ValueExpected(const T& value):
            value(value)
        {}
    };

    template<typename T, const std::string_view& name>
    struct MemberValueExpected: public ValueExpected<T>
    {
        static const constexpr decltype(name) memberName = name;
        explicit MemberValueExpected(const T& value):
                ValueExpected<T>(value)
        {}
    };

    template<typename T>
    requires(std::is_trivial_v<T>)
    void checkValue(T value, T expected)
    {
        REQUIRE(value == expected);
    }

    template<typename... Ts>
    void checkValue(const ObjectRepresentation& object,  std::tuple<Ts...> expectedValue)
    {
        checkValue(object, expectedValue, std::index_sequence_for<Ts...>{});
    }

    template<typename T, std::size_t... Is>
    void checkValue(const ObjectRepresentation& object, T expectedValue, std::index_sequence<Is...>)
    {
        (checkMemberValue<std::tuple_element_t<Is, T>>(object, std::get<Is>(expectedValue).value), ...);
    }

    template<typename TExpected, typename T>
    void checkMemberValue(const ObjectRepresentation& object, T objectMemberValue)
    {
        using MemberTypeExpected = typename TExpected::Type;
        auto memberIterator = object.find((std::string)TExpected::memberName);
        REQUIRE(memberIterator != std::end(object));
        assert(memberIterator->second);
        auto& value = memberIterator->second->value;
        REQUIRE(std::holds_alternative<MemberTypeExpected>(value));
        checkValue(std::get<MemberTypeExpected>(value), objectMemberValue);
    }

    TEST_CASE( "Visit object tree", "[objectVisitor]" )
    {
        const constexpr Point point{1, 2, 3};

        auto rootValueNode = std::make_shared<ValueNode>(nullptr);
        VisitorContext context(rootValueNode);
        ObjectVisitor<VisitorPolicies>::visit(point, context);

        REQUIRE(std::holds_alternative<ObjectRepresentation>(rootValueNode->value));
        auto& pointRepresentation = std::get<ObjectRepresentation>(rootValueNode->value);

        static const constexpr std::string_view xMember = "x";
        static const constexpr std::string_view yMember = "y";
        static const constexpr std::string_view zMember = "z";

        auto expected = std::tuple{
            MemberValueExpected<float, xMember>(point.x),
            MemberValueExpected<float, yMember>(point.y),
            MemberValueExpected<float, zMember>(point.z)
        };
        checkValue(pointRepresentation, expected);
    }
};