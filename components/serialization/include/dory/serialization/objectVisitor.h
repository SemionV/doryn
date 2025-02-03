#pragma once

#include <queue>

#include "reflection.h"
#include <dory/generic/typeTraits.h>

namespace dory::serialization
{
    template<typename TRegistry, typename TDataContext>
    struct Context
    {
        TRegistry& registry;
        TDataContext& dataContext;

        Context(TRegistry& registry, TDataContext& dataContext):
            registry(registry), dataContext(dataContext)
        {}

        Context(const Context& other):
            registry(other.registry),
            dataContext(other.dataContext)
        {}

        Context& operator=(const Context& other)
        {
            return *this;
        }
    };

    template<typename TNode, typename TRegistry, typename TDataContext>
    struct TreeStructureContext: Context<TRegistry, TDataContext>
    {
        using NodeType = TNode;

        TNode node;
        std::size_t collectionIndex {};
        std::size_t collectionSize {};

        TreeStructureContext() = default;

        explicit TreeStructureContext(TNode node, TRegistry& registry, TDataContext& dataContext):
            Context<TRegistry, TDataContext>(registry, dataContext),
            node(node)
        {}

        TreeStructureContext(const TreeStructureContext& other) = default;
    };

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
        inline static std::optional<TContext> beginMember(auto&& member, const std::size_t i, TContext& context)
        {
            return TContext{ context };
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
        inline static std::optional<TContext> beginItem(const std::size_t i, TContext& context)
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

        template<typename T, typename TItem, typename TContext>
        inline static std::optional<TContext> nextItem(T& collection, TItem**, TContext& context)
        {
            return {};
        }

        template<typename V, typename TContext>
        inline static void endItem(auto& item, V& collection, TContext& context)
        {
        }
    };

    template<typename TDerived>
    struct ContainerPolicy
    {
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

        template<typename TCollection, typename TContext>
        static void beginCollection(TCollection&& collection, TContext& context)
        {
            context.collectionIndex = 0;
            TDerived::setCollectionSize(collection, context.node, context.collectionSize);
        }

        template<typename TCollection, typename TItem, typename TContext>
        static auto nextItem(TCollection& collection, TItem** item, TContext& context)
        {
            if(itemsLeft(context))
            {
                auto itemContext = TDerived::getCollectionItem(context, collection, context.collectionIndex, context.node, item);
                ++context.collectionIndex;
                return itemContext;
            }

            *item = nullptr;
            return decltype(TDerived::getCollectionItem(context, collection, context.collectionIndex, context.node, item)){};
        }

        template<typename TContext>
        static bool itemsLeft(TContext& context)
        {
            return context.collectionIndex < context.collectionSize;
        }

        template<typename TCollection, typename TContext>
        static void endItem(auto& item, TCollection& collection, TContext& context)
        {}

        template<typename TCollection, typename TContext>
        static void endCollection(TCollection& collection, TContext& context)
        {}
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

        template<typename T>
        struct CollectionItemType;

        template<typename T>
        requires(generic::is_dynamic_collection_v<T>)
        struct CollectionItemType<T>
        {
            using Type = typename std::decay_t<T>::value_type;
        };

        template<typename T>
        requires(generic::is_dynamic_collection_v<T>)
        struct CollectionItemType<const T>
        {
            using Type = const typename std::decay_t<T>::value_type;
        };

        template<typename T>
        requires(generic::is_dictionary_v<T>)
        struct CollectionItemType<T>
        {
            using Type = typename std::decay_t<T>::mapped_type;
        };

        template<typename T>
        requires(generic::is_dictionary_v<T>)
        struct CollectionItemType<const T>
        {
            using Type = const typename std::decay_t<T>::mapped_type;
        };

        template<typename T>
        using CollectionItemTypeT = typename CollectionItemType<T>::Type;
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
                if(auto itemContext = TPolicies::CollectionItemPolicy::beginItem(i, context))
                {
                    TVisitor::visit(array[i], *itemContext);
                    TPolicies::CollectionItemPolicy::endItem(i == lastIndex, context);
                }
            }

            TPolicies::CollectionPolicy::endCollection(context);
        }
    };

    template<typename TPolicies, typename TVisitor>
    struct ClassVisitor
    {
        template<typename T, typename TContext>
        static void visit(T&& object, TContext& context)
        {
            if(TPolicies::ObjectPolicy::beginObject(std::forward<T>(object), context))
            {
                reflection::visitClassFields(object, []<typename TClassMember>(TClassMember&& member, const std::size_t i, const std::size_t memberCount, TContext& ctx)
                {
                    if(auto memberContext = TPolicies::MemberPolicy::beginMember(std::forward<TClassMember>(member), i, ctx))
                    {
                        TVisitor::visit(member.value, *memberContext);
                        TPolicies::MemberPolicy::endMember(i == memberCount - 1, ctx);
                    }
                }, context);

                TPolicies::ObjectPolicy::endObject(std::forward<T>(object), context);
            }
        }
    };

    template<typename TPolicies = VisitorDefaultPolicies, typename... TBaseVisitors>
    class ObjectVisitor: public TBaseVisitors...
    {
    public:
        using TBaseVisitors::visit...;

        template<typename T, typename TContext>
        requires(std::is_fundamental_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(std::is_enum_v<std::remove_reference_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::EnumPolicy::process(std::forward<T>(object), context);
        }

        template<typename TContext>
        static void visit(std::string& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object, context);
        }

        template<typename TContext>
        static void visit(const std::string& object, TContext& context)
        {
            TPolicies::ValuePolicy::process(object, context);
        }

        template<typename T, typename TContext>
        requires(generic::is_optional_v<std::decay_t<T>>)
        static void visit(T&& object, TContext& context)
        {
            visit(*object, context);
        }

        template<typename T, auto size, typename TContext>
        static void visit(std::array<T, size>& object, TContext& context)
        {
            ArrayVisitor<T, size, TPolicies, ObjectVisitor>::visit(object, context);
        }

        template<typename T, auto size, typename TContext>
        static void visit(const std::array<T, size>& object, TContext& context)
        {
            ArrayVisitor<const T, size, TPolicies, ObjectVisitor>::visit(object, context);
        }

        template<typename T, typename TContext>
        requires(generic::is_dynamic_collection_v<T> || generic::is_dictionary_v<T>)
        static void visit(T&& object, TContext& context)
        {
            TPolicies::ContainerPolicyType::beginCollection(std::forward<T>(object), context);

            using ItemType = typename TPolicies::Traits::template CollectionItemTypeT<std::remove_reference_t<T>>;

            ItemType* item;
            auto itemContext = TPolicies::ContainerPolicyType::nextItem(std::forward<T>(object), &item, context);
            while(itemContext && item)
            {
                visit(*item, *itemContext);
                TPolicies::ContainerPolicyType::endItem(*item, std::forward<T>(object), context);
                itemContext = TPolicies::ContainerPolicyType::nextItem(std::forward<T>(object), &item, context);
            }

            TPolicies::ContainerPolicyType::endCollection(std::forward<T>(object), context);
        }

        template<typename T, typename TContext>
        requires(reflection::IsReflectableV<std::decay_t<std::remove_reference_t<T>>> && std::is_class_v<std::decay_t<std::remove_reference_t<T>>>)
        static void visit(T&& object, TContext& context)
        {
            ClassVisitor<TPolicies, ObjectVisitor>::visit(std::forward<T>(object), context);
        }
    };

    template<typename TPolicies, typename... TBaseVisitors>
    class ObjectVisitor<TPolicies, generic::TypeList<TBaseVisitors...>>: public ObjectVisitor<TPolicies, TBaseVisitors...>
    {};
}
