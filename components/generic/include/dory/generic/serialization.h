#pragma once

namespace dory::generic::serialization
{
    template<typename TRegistry, typename TDataContext, typename TDataFormat>
    struct ContextPolicies
    {
        using RegistryType = TRegistry;
        using DataContextType = TDataContext;
        using DataFormatType = TDataFormat;
    };

    template<typename TContextPolicies>
    struct Context
    {
        typename TContextPolicies::RegistryType& registry;
        typename TContextPolicies::DataContextType& dataContext;
        typename TContextPolicies::DataFormatType dataFormat;

        Context(typename TContextPolicies::RegistryType& registry, typename TContextPolicies::DataContextType& dataContext, typename TContextPolicies::DataFormatType dataFormat):
            registry(registry), dataContext(dataContext), dataFormat(dataFormat)
        {}

        Context(const Context& other):
            registry(other.registry),
            dataContext(other.dataContext),
            dataFormat(other.dataFormat)
        {}

        Context(Context&& other) noexcept:
            registry(other.registry),
            dataContext(other.dataContext),
            dataFormat(other.dataFormat)
        {}

        Context& operator=(const Context& other)
        {
            return *this;
        }
    };

    template<typename TNode, typename TContextPolicies>
    struct TreeStructureContext: Context<TContextPolicies>
    {
        using NodeType = TNode;

        TNode node;
        std::size_t collectionIndex {};
        std::size_t collectionSize {};

        TreeStructureContext() = default;

        explicit TreeStructureContext(TNode node, const Context<TContextPolicies>& otherContext):
            Context<TContextPolicies>(otherContext),
            node(node)
        {}

        explicit TreeStructureContext(TNode node, Context<TContextPolicies>&& otherContext):
            Context<TContextPolicies>(otherContext),
            node(node)
        {}

        TreeStructureContext(const TreeStructureContext& other) = default;
    };
}
