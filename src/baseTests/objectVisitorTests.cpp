#include <utility>

#include "baseTests/dependencies.h"
#include "base/serialization/objectVisitor.h"

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

    struct ValuePolicyTest
    {
        template<typename T, typename TContext>
        inline static void process(T&& value, TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode->value = value;
        }
    };

    struct ObjectPolicyTest
    {
        template<typename TContext>
        inline static void beginObject(TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode->value = ObjectRepresentation();
        }

        template<typename TContext>
        inline static void endObject(TContext& context)
        {
        }
    };

    struct MemberPolicyTest
    {
        template<typename TContext>
        inline static bool beginMember(const std::string_view& memberName, const std::size_t i, TContext& context)
        {
            assert(std::holds_alternative<ObjectRepresentation>(context.currentValueNode->value));
            auto valueNode = std::make_shared<ValueNode>(context.currentValueNode);
            std::get<ObjectRepresentation>(context.currentValueNode->value).emplace(memberName, valueNode);
            context.currentValueNode = valueNode;

            return true;
        }

        template<typename TContext>
        inline static void endMember(const bool lastMember, TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode = context.currentValueNode->parent;
            assert(std::holds_alternative<ObjectRepresentation>(context.currentValueNode->value));
        }
    };

    struct CollectionPolicyTest
    {
        template<typename T, auto N, typename TContext>
        inline static void beginCollection(TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode->value = CollectionRepresentation();
        }

        template<typename TContext>
        inline static void endCollection(TContext& context)
        {
        }
    };

    struct CollectionItemPolicyTest
    {
        template<typename TContext>
        inline static void beginItem(const std::size_t i, TContext& context)
        {
            assert(std::holds_alternative<CollectionRepresentation>(context.currentValueNode->value));
            auto valueNode = std::make_shared<ValueNode>(context.currentValueNode);
            std::get<CollectionRepresentation>(context.currentValueNode->value).emplace_back(valueNode);
            context.currentValueNode = valueNode;
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
            assert(context.currentValueNode);
            context.currentValueNode = context.currentValueNode->parent;
            assert(std::holds_alternative<CollectionRepresentation>(context.currentValueNode->value));
        }
    };

    struct VisitorPolicies
    {
        using ValuePolicy = ValuePolicyTest;
        using ObjectPolicy = ObjectPolicyTest;
        using MemberPolicy = MemberPolicyTest;
        using CollectionPolicy = CollectionPolicyTest;
        using CollectionItemPolicy = CollectionItemPolicyTest;
    };

    template<typename TValue, typename TExpected = TValue>
    struct ValueExpected
    {
        using Type = TValue;
        TExpected value;
        explicit ValueExpected(TExpected value) noexcept:
            value(std::move(value))
        {}

        ValueExpected() = default;
    };

    template<auto name, typename TValue, typename TExpected = TValue>
    struct MemberValueExpected: public ValueExpected<TValue, TExpected>
    {
        static const constexpr decltype(name) memberName = name;
        explicit MemberValueExpected(const TExpected& value) noexcept:
                ValueExpected<TValue, TExpected>(value)
        {}

        MemberValueExpected() = default;
    };

    template<typename T, auto N>
    void checkValue(const CollectionRepresentation& collection,  std::array<T, N> expectedValue);

    template<typename T>
    requires(std::is_trivial_v<T>)
    void checkValue(T value, const ValueExpected<T, T>& expectedValue)
    {
        REQUIRE(value == expectedValue.value);
    }

    template<typename TValue, typename TExpected>
    void checkValue(const ObjectRepresentation& object,  const ValueExpected<TValue, TExpected>& expectedValue)
    {
        checkValue(object, expectedValue.value);
    }

    template<typename TValue, typename TExpected>
    void checkValue(const CollectionRepresentation& object,  const ValueExpected<TValue, TExpected>& expectedValue)
    {
        checkValue(object, expectedValue.value);
    }

    template<typename... Ts>
    void checkValue(const ObjectRepresentation& object,  const std::tuple<Ts...>& expectedValue)
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

    template<typename T, auto N>
    void checkValue(const CollectionRepresentation& collection,  std::array<T, N> expectedValue)
    {
        REQUIRE(collection.size() == N);
        using MemberTypeExpected = typename T::Type;

        for(std::size_t i = 0; i < N; ++i)
        {
            const auto& nodeItem = collection[i];
            auto& value = nodeItem->value;
            REQUIRE(std::holds_alternative<MemberTypeExpected>(value));
            checkValue(std::get<MemberTypeExpected>(value), expectedValue[i]);
        }
    }

    template<typename T>
    requires(std::is_fundamental_v<T>)
    constexpr auto makeExpectation(T expectedValue)
    {
        return ValueExpected<T>(expectedValue);
    }

    template<typename T, auto N>
    constexpr decltype(auto) makeExpectation(const std::array<T, N>& collection)
    {
        using ExpectationType = decltype(makeExpectation(std::declval<T>()));

        auto collectionExpectation = std::array<ExpectationType, N>{};
        for(std::size_t i = 0; i < N; ++i)
        {
            collectionExpectation[i] = makeExpectation(collection[i]);
        }

        return ValueExpected<CollectionRepresentation, decltype(collectionExpectation)>{collectionExpectation};
    }

    template<auto propertyName, typename T, typename TExpected = T>
    requires(std::is_trivial_v<T> || !std::is_same_v<T, TExpected>)
    constexpr decltype(auto) makeExpectation(const T& propertyValue)
    {
        auto valueExpectation = makeExpectation(propertyValue);
        return MemberValueExpected<propertyName, TExpected, decltype(valueExpectation)>(valueExpectation);
    }

    template<auto propertyName, typename T>
    requires(std::is_class_v<T>)
    constexpr decltype(auto) makeExpectation(const T& propertyValue)
    {
        return makeExpectation<propertyName, T, ObjectRepresentation>(propertyValue);
    }

    template<auto propertyName, typename T, auto N>
    constexpr decltype(auto) makeExpectation(const std::array<T, N>& propertyValue)
    {
        return makeExpectation<propertyName, std::array<T, N>, CollectionRepresentation>(propertyValue);
    }

    template<typename... TPropertyExpecations>
    constexpr decltype(auto) makeExpectation(TPropertyExpecations... propertyExpectations)
    {
        auto objectTuple = std::tuple{ propertyExpectations... };
        return ValueExpected<ObjectRepresentation, decltype(objectTuple)>{objectTuple};
    }

    constexpr decltype(auto) makeExpectation(const Point& point)
    {
        return makeExpectation(makeExpectation<reflection::makeConstString("x")>(point.x),
                               makeExpectation<reflection::makeConstString("y")>(point.y),
                               makeExpectation<reflection::makeConstString("z")>(point.z));
    }

    constexpr decltype(auto) makeExpectation(const Axises& axises)
    {
        return makeExpectation(makeExpectation<reflection::makeConstString("i")>(axises.i),
                               makeExpectation<reflection::makeConstString("j")>(axises.j),
                               makeExpectation<reflection::makeConstString("k")>(axises.k));
    }

    constexpr decltype(auto) makeExpectation(const AffineTransformation& transformation)
    {
        return makeExpectation(makeExpectation<reflection::makeConstString("translation")>(transformation.translation),
                               makeExpectation<reflection::makeConstString("rotation")>(transformation.rotation),
                               makeExpectation<reflection::makeConstString("axises")>(transformation.axises));
    }

    template<typename T, typename TRepresentation = T>
    void checkVisitorRepresentation(T value)
    {
        auto rootValueNode = std::make_shared<ValueNode>(nullptr);
        VisitorContext context(rootValueNode);
        ObjectVisitor<VisitorPolicies>::visit(value, context);

        REQUIRE(std::holds_alternative<TRepresentation>(rootValueNode->value));
        auto& representation = std::get<TRepresentation>(rootValueNode->value);

        auto expected = makeExpectation(value);
        checkValue(representation, expected);
    }

    template<typename T>
    requires(std::is_trivial_v<T>)
    void checkVisitor(const T& value)
    {
        checkVisitorRepresentation<T>(value);
    }

    template<typename T>
    requires(std::is_class_v<T>)
    void checkVisitor(const T& value)
    {
        checkVisitorRepresentation<T, ObjectRepresentation>(value);
    }

    template<typename T, auto N>
    void checkVisitor(const std::array<T, N>& value)
    {
        checkVisitorRepresentation<std::array<T, N>, CollectionRepresentation>(value);
    }

    TEST_CASE( "Visit object tree shallow", "[objectVisitor]" )
    {
        const constexpr Point point{1, 2, 3};
        checkVisitor(point);
    }

    TEST_CASE( "Visit object tree deep", "[objectVisitor]" )
    {
        const constexpr Point axisI{1, 2, 3};
        const constexpr Point axisJ{4, 5, 6};
        const constexpr Point axisK{7, 8, 9};
        const constexpr Axises axises{axisI, axisJ, axisK};

        checkVisitor(axises);
    }

    TEST_CASE( "Visit collection", "[objectVisitor]" )
    {
        std::array<float, 3> values = {1, 2, 3};
        checkVisitor(values);
    }

    TEST_CASE( "Visit object with collection members", "[objectVisitor]" )
    {
        auto translation = Point{1, 2, 3};
        auto rotation = std::array<float, 9>{4, 5, 6, 7, 8, 9, 10, 11, 12};
        auto axises = std::array<Point, 3>{Point{1, 2, 3}, Point{4, 5, 6}, Point{7, 8, 9}};
        auto transformation = AffineTransformation{translation, rotation, axises};

        checkVisitor(transformation);
    }

    TEST_CASE( "Visit cellection of collections", "[objectVisitor]" )
    {
        auto pointX = std::array<float, 3>{1, 2, 3};
        auto pointY = std::array<float, 3>{4, 5, 6};
        auto pointZ = std::array<float, 3>{7, 8, 9};
        auto collection = std::array<decltype(pointX), 3>{pointX, pointY, pointZ};

        checkVisitor(collection);
    }

    TEST_CASE( "Visit primitive value", "[objectVisitor]" )
    {
        std::size_t value = 11;
        checkVisitor(value);
    }
};