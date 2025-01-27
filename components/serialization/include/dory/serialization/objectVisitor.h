#pragma once

#include <stack>
#include <queue>

#include "reflection.h"
#include <dory/generic/parameterizedString.h>
#include <dory/generic/typeTraits.h>

namespace dory::serialization
{
    struct DefaultValuePolicy
    {
        template<typename T, typename TContext>
        inline static void process(T&& value, TContext& context)
        {
        }
    };

    struct DefaultEnumPolicy
    {
        template<typename T, typename TContext>
        static void process(T&& value, TContext& context)
        {
        }
    };

    struct DefaultObjectPolicy
    {
        template<typename TContext, typename T>
        inline static bool beginObject(T&& object, TContext& context)
        {
            return true;
        }

        template<typename TContext, typename T>
        inline static void endObject(T&& object, TContext& context)
        {
        }
    };

    struct DefaultMemberPolicy
    {
        template<typename TContext>
        inline static bool beginMember(auto&& member, const std::size_t i, TContext& context)
        {
            return true;
        }

        template<typename TContext>
        inline static void endMember(const bool lastMember, TContext& context)
        {
        }
    };

    struct DefaultCollectionPolicy
    {
        template<typename T, auto N, typename TCollection, typename TContext>
        inline static void beginCollection(TCollection& collection, TContext& context)
        {
        }

        template<typename TContext>
        inline static void endCollection(TContext& context)
        {
        }
    };

    struct DefaultCollectionItemPolicy
    {
        template<typename TContext>
        inline static bool beginItem(const std::size_t i, TContext& context)
        {
            return true;
        }

        template<typename TContext>
        inline static void endItem(const bool lastItem, TContext& context)
        {
        }
    };

    struct DefaultContainerPolicy
    {
        template<typename T, typename TContext>
        inline static void beginCollection(T& collection, TContext& context)
        {
        }

        template<typename T, typename TContext>
        inline static void endCollection(T& collection, TContext& context)
        {
        }

        template<typename T, typename TContext>
        inline static std::optional<std::reference_wrapper<generic::GetCollectionValueType<T>>> nextItem(T& collection, TContext& context)
        {
            return {};
        }

        template<typename V, typename TContext>
        inline static void endItem(auto& item, V& collection, TContext& context)
        {
        }
    };

    template<typename TNode, typename... Ts>
    struct TreeStructureContext: public Ts...
    {
        std::stack<TNode> parents;
        std::stack<std::size_t> collectionIndexesStack;
        std::stack<std::size_t> collectionSizesStack;

        explicit TreeStructureContext(TNode data)
        {
            parents.push(data);
        }
    };

    template<typename TDerived, typename TContext>
    struct ContainerPolicy;

    template<typename TDerived, typename TNode, typename... Ts>
    struct ContainerPolicy<TDerived, TreeStructureContext<TNode, Ts...>>
    {
        using NodeType = TNode;
        using ContextType = TreeStructureContext<TNode, Ts...>;

        template<typename TCollection>
        static auto getKeyValue(const auto& key)
        {
            return typename TCollection::key_type { key }; //string key
        }

        template<typename TCollection>
        requires(std::is_enum_v<typename TCollection::key_type>)
        static auto getKeyValue(const auto& key)
        {
            auto enumValue = magic_enum::enum_cast<typename TCollection::key_type>(key);
            if(enumValue)
            {
                return *enumValue;
            }

            return typename TCollection::key_type{};
        }

        template<typename T>
        static auto getKeyString(const T& key)
        {
            return std::string_view { key };
        }

        template<typename T>
        requires(std::is_enum_v<T>)
        static auto getKeyString(const T& key)
        {
            return magic_enum::enum_name(key);
        }

        template<typename TCollection>
        static auto& getMapItem(TCollection& collection, const auto& index)
        {
            size_t currentIndex = 0;

            for(const auto& pair : collection)
            {
                if (currentIndex == index)
                {
                    return pair;
                }

                ++currentIndex;
            }

            assert(false);
        }

        template<typename TCollection>
        static void beginCollection(TCollection&& collection, ContextType& context)
        {
            context.collectionIndexesStack.emplace(0);
            auto& size = context.collectionSizesStack.emplace(0);
            TDerived::setCollectionSize(collection, context.parents, size);
        }

        template<typename TCollection>
        static auto nextItem(TCollection& collection, ContextType& context)
        {
            if(itemsLeft<TCollection>(context))
            {
                auto& item = getItem(collection, context);
                return std::optional{std::ref(item)};
            }

            using ValueType = decltype(getItem(std::declval<TCollection&>(), std::declval<ContextType&>()));
            return std::optional<decltype(std::ref(std::declval<ValueType>()))>{};
        }

        template<typename TCollection>
        static bool itemsLeft(ContextType& context)
        {
            return context.collectionIndexesStack.top() < context.collectionSizesStack.top();
        }

        template<typename TCollection>
        static auto& getItem(TCollection& collection, ContextType& context)
        {
            auto& index = context.collectionIndexesStack.top();
            auto& item = TDerived::getCollectionItem(collection, index, context.parents);
            ++index;
            return item;
        }

        template<typename T>
        static void endItem(auto& item, T& collection, ContextType& context)
        {
            context.parents.pop();
        }

        template<typename TCollection>
        static void endCollection(TCollection& collection, ContextType& context)
        {
            context.collectionIndexesStack.pop();
            context.collectionSizesStack.pop();
        }
    };

    struct VisitorTypeTraitPolicies
    {
        template<typename T>
        constexpr static bool IsFundamentalV = std::is_fundamental_v<std::decay_t<T>>;

        template<typename T>
        constexpr static bool IsEnumV = std::is_enum_v<std::decay_t<T>>;

        template<typename T, typename U>
        constexpr static bool IsSameV = std::is_same_v<std::decay_t<T>, U>;

        template<typename T, typename U>
        constexpr static bool IsBaseOfV = std::is_base_of_v<T, std::decay_t<U>>;

        template<typename T>
        constexpr static bool IsOptionalV = generic::is_optional_v<std::decay_t<T>>;

        template<typename T>
        constexpr static bool IsArrayV = generic::is_fixed_array_v<std::decay_t<T>>;

        template<typename T>
        constexpr static bool IsContainerV = generic::is_dynamic_collection_v<T> || generic::is_dictionary_v<T>;
    };

    struct VisitorDefaultPolicies
    {
        using ValuePolicy = DefaultValuePolicy;
        using EnumPolicy = DefaultEnumPolicy;
        using ObjectPolicy = DefaultObjectPolicy;
        using MemberPolicy = DefaultMemberPolicy;
        using CollectionPolicy = DefaultCollectionPolicy;
        using CollectionItemPolicy = DefaultCollectionItemPolicy;
        using ContainerPolicyType = DefaultContainerPolicy;
        using Traits = VisitorTypeTraitPolicies;
    };

    template<typename TPolicies = VisitorDefaultPolicies>
    struct DefaultVisitor;

    template<typename T, typename TPolicies, typename TVisitor = DefaultVisitor<>>
    struct Visitor;

    template<typename T, typename TPolicies, typename TVisitor>
    requires(reflection::IsReflectableV<std::decay_t<T>> && std::is_class_v<std::decay_t<T>>)
    struct Visitor<T, TPolicies, TVisitor>
    {
        template<typename C, typename TContext>
        static void visit(C&& object, TContext& context)
        {
            using ObjectType = decltype(object);
            if(TPolicies::ObjectPolicy::beginObject(std::forward<C>(object), context))
            {
                reflection::visitClassFields<ObjectType>(std::forward<C>(object), []<typename TMember>(reflection::ClassMember<ObjectType, TMember>& member, const std::size_t i, const std::size_t memberCount, TContext& ctx)
                {
                    if(TPolicies::MemberPolicy::beginMember(member, i, ctx))
                    {
                        TVisitor::visit(member.value, ctx);
                        TPolicies::MemberPolicy::endMember(i == memberCount - 1, ctx);
                    }
                }, context);

                TPolicies::ObjectPolicy::endObject(std::forward<C>(object), context);
            }
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    requires(TPolicies::Traits::template IsFundamentalV<T>)
    struct Visitor<T, TPolicies, TVisitor>
    {
        template<typename F, typename TContext>
        static void visit(F&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(std::forward<F>(object), context);
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    requires(TPolicies::Traits::template IsEnumV<T>)
    struct Visitor<T, TPolicies, TVisitor>
    {
        template<typename E, typename TContext>
        static void visit(E&& object, TContext& context)
        {
            TPolicies::EnumPolicy::process(std::forward<E>(object), context);
        }
    };

    template<typename TPolicies, typename TVisitor>
    struct Visitor<std::string, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(std::string& value, TContext& context)
        {
            TPolicies::ValuePolicy::process(value, context);
        }
    };

    template<typename TPolicies, typename TVisitor>
    struct Visitor<const std::string, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(const std::string& value, TContext& context)
        {
            TPolicies::ValuePolicy::process(value, context);
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    requires(TPolicies::Traits::template IsBaseOfV<generic::IParameterizedString, std::decay_t<T>>)
    struct Visitor<T, TPolicies, TVisitor>
    {
        template<typename P, typename TContext>
        static void visit(P&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object.getTemplate(), context);
            object.updateTemplate();
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    requires(TPolicies::Traits::template IsBaseOfV<generic::IParameterizedString, T>)
    struct Visitor<std::unique_ptr<T>, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(std::unique_ptr<T>& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object->getTemplate(), context);
            object->updateTemplate();
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    struct Visitor<std::optional<T>, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(std::optional<T>& object, TContext& context)
        {
            TVisitor::visit(*object, context);
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    struct Visitor<const std::optional<T>, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(const std::optional<T>& object, TContext& context)
        {
            TVisitor::visit(*object, context);
        }
    };

    template<typename TValue, auto size, typename TPolicies, typename TVisitor>
    struct ArrayVisitor
    {
        template<typename A, typename TContext>
        static void visit(A&& array, TContext& context)
        {
            TPolicies::CollectionPolicy::template beginCollection<TValue, size>(std::forward<A>(array), context);

            const size_t lastIndex = size - 1;
            for(std::size_t i {}; i < size; ++i)
            {
                if(TPolicies::CollectionItemPolicy::beginItem(i, context))
                {
                    TVisitor::visit(array[i], context);
                    TPolicies::CollectionItemPolicy::endItem(i == lastIndex, context);
                }
            }

            TPolicies::CollectionPolicy::endCollection(context);
        }
    };

    template<typename T, auto size, typename TPolicies, typename TVisitor>
    struct Visitor<std::array<T, size>, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(std::array<T, size>& object, TContext& context)
        {
            ArrayVisitor<T, size, TPolicies, TVisitor>::visit(object, context);
        }
    };

    template<typename T, auto size, typename TPolicies, typename TVisitor>
    struct Visitor<const std::array<T, size>, TPolicies, TVisitor>
    {
        template<typename TContext>
        static void visit(const std::array<T, size>& object, TContext& context)
        {
            ArrayVisitor<const T, size, TPolicies, TVisitor>::visit(object, context);
        }
    };

    template<typename T, typename TPolicies, typename TVisitor>
    requires(TPolicies::Traits::template IsContainerV<T>)
    struct Visitor<T, TPolicies, TVisitor>
    {
        template<typename TContainer, typename TContext>
        static void visit(TContainer&& object, TContext& context)
        {
            TPolicies::ContainerPolicyType::beginCollection(std::forward<TContainer>(object), context);

            auto item = TPolicies::ContainerPolicyType::nextItem(std::forward<TContainer>(object), context);
            while(item)
            {
                auto valueRef = *item;

                TVisitor::visit(valueRef.get(), context);
                TPolicies::ContainerPolicyType::endItem(valueRef, std::forward<TContainer>(object), context);
                item = TPolicies::ContainerPolicyType::nextItem(std::forward<TContainer>(object), context);
            }

            TPolicies::ContainerPolicyType::endCollection(std::forward<TContainer>(object), context);
        }
    };

    template<typename TPolicies>
    struct DefaultVisitor
    {
        template<typename T, typename TContext>
        static void visit(T&& object, TContext& context)
        {
            Visitor<std::remove_reference_t<T>, TPolicies, DefaultVisitor>::visit(std::forward<T>(object), context);
        }
    };

    template<typename TPolicies = VisitorDefaultPolicies>
    class ObjectVisitor
    {
    public:
        template<typename T, typename TContext>
        static void visit(T&& object, TContext& context)
        {
            DefaultVisitor<TPolicies>::visit(std::forward<T>(object), context);
        }
    };
}
