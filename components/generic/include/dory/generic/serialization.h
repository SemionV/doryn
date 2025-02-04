#pragma once

namespace dory::generic::serialization
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
}
