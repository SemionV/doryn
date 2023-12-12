#include <utility>

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
            std::get<CollectionRepresentation>(context.currentValueNode->value).emplace_back(valueNode);
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
            checkValue(std::get<MemberTypeExpected>(value), expectedValue[i].value);
        }
    }

    template<typename T>
    requires(std::is_fundamental_v<T>)
    auto makeExpectation(T expectedValue)
    {
        return ValueExpected<T>(expectedValue);
    }

    template<typename T, auto N>
    decltype(auto) makeExpectation(const std::array<T, N>& collection)
    {
        using ExpectationItemType = std::conditional_t<std::is_trivial_v<T>,
                T,
                decltype(makeExpectation(std::declval<T>()))>;

        auto expectation = std::array<ValueExpected<ExpectationItemType>, N>{};
        for(std::size_t i = 0; i < N; ++i)
        {
            expectation[i] = ValueExpected<ExpectationItemType>(makeExpectation(collection[i]));
        }

        return expectation;
    }

    decltype(auto) makeExpectation(const Point& point)
    {
        return std::tuple{
                MemberValueExpected<reflection::makeConstString("x"), float>(point.x),
                MemberValueExpected<reflection::makeConstString("y"), float>(point.y),
                MemberValueExpected<reflection::makeConstString("z"), float>(point.z)
        };
    }

    decltype(auto) makeExpectation(const Axises& axises)
    {
        auto axisIExpectation = makeExpectation(axises.i);
        auto axisJExpectation = makeExpectation(axises.j);
        auto axisKExpectation = makeExpectation(axises.k);

        return std::tuple{
                MemberValueExpected<reflection::makeConstString("i"), ObjectRepresentation, decltype(axisIExpectation)>(axisIExpectation),
                MemberValueExpected<reflection::makeConstString("j"), ObjectRepresentation, decltype(axisJExpectation)>(axisJExpectation),
                MemberValueExpected<reflection::makeConstString("k"), ObjectRepresentation, decltype(axisKExpectation)>(axisKExpectation)
        };
    }

    decltype(auto) makeExpectation(const AffineTransformation& transformation)
    {
        auto translationExpectation = makeExpectation(transformation.translation);
        auto rotationExpectation = makeExpectation(transformation.rotation);
        auto axisesExpectation = makeExpectation(transformation.axises);

        return std::tuple{
                MemberValueExpected<reflection::makeConstString("translation"), ObjectRepresentation, decltype(translationExpectation)>(translationExpectation),
                MemberValueExpected<reflection::makeConstString("rotation"), CollectionRepresentation, decltype(rotationExpectation)>(rotationExpectation)/*,
                MemberValueExpected<reflection::makeConstString("axises"), CollectionRepresentation, decltype(axisesExpectation)>(axisesExpectation)*/
        };
    }

    TEST_CASE( "Visit object tree", "[objectVisitor]" )
    {
        const constexpr Point point{1, 2, 3};

        auto rootValueNode = std::make_shared<ValueNode>(nullptr);
        VisitorContext context(rootValueNode);
        ObjectVisitor<VisitorPolicies>::visit(point, context);

        REQUIRE(std::holds_alternative<ObjectRepresentation>(rootValueNode->value));
        auto& pointRepresentation = std::get<ObjectRepresentation>(rootValueNode->value);

        auto expected = std::tuple{
            MemberValueExpected<reflection::makeConstString("x"), float>(point.x),
            MemberValueExpected<reflection::makeConstString("y"), float>(point.y),
            MemberValueExpected<reflection::makeConstString("z"), float>(point.z)
        };
        checkValue(pointRepresentation, expected);
    }

    TEST_CASE( "Visit object tree deep", "[objectVisitor]" )
    {
        const constexpr Point axisI{1, 2, 3};
        const constexpr Point axisJ{4, 5, 6};
        const constexpr Point axisK{7, 8, 9};
        const constexpr Axises axises{axisI, axisJ, axisK};

        auto rootValueNode = std::make_shared<ValueNode>(nullptr);
        VisitorContext context(rootValueNode);
        ObjectVisitor<VisitorPolicies>::visit(axises, context);

        REQUIRE(std::holds_alternative<ObjectRepresentation>(rootValueNode->value));
        auto& axisesRepresentation = std::get<ObjectRepresentation>(rootValueNode->value);

        auto expected = makeExpectation(axises);
        checkValue(axisesRepresentation, expected);
    }

    TEST_CASE( "Visit collection", "[objectVisitor]" )
    {
        std::array<float, 3> values = {1, 2, 3};

        auto rootValueNode = std::make_shared<ValueNode>(nullptr);
        VisitorContext context(rootValueNode);
        ObjectVisitor<VisitorPolicies>::visit(values, context);

        REQUIRE(std::holds_alternative<CollectionRepresentation>(rootValueNode->value));
        auto& valuesRepresentation = std::get<CollectionRepresentation>(rootValueNode->value);

        auto expected = makeExpectation(values);
        checkValue(valuesRepresentation, expected);
    }

    TEST_CASE( "Visit object with collection members", "[objectVisitor]" )
    {
        auto translation = Point{1, 2, 3};
        auto rotation = std::array<float, 9>{4, 5, 6, 7, 8, 9, 10, 11, 12};
        auto axises = std::array<Point, 3>{Point{1, 2, 3}, Point{4, 5, 6}, Point{7, 8, 9}};
        auto transformation = AffineTransformation{translation, rotation, axises};

        auto rootValueNode = std::make_shared<ValueNode>(nullptr);
        VisitorContext context(rootValueNode);
        ObjectVisitor<VisitorPolicies>::visit(transformation, context);

        REQUIRE(std::holds_alternative<ObjectRepresentation>(rootValueNode->value));
        auto& transformationRepresentation = std::get<ObjectRepresentation>(rootValueNode->value);

        auto expected = makeExpectation(transformation);
        checkValue(transformationRepresentation, expected);
    }
};