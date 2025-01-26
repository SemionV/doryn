#pragma once

namespace dory::serialization::copy
{
    struct ObjectCopyContext: TreeStructureContext<void*>
    {
        explicit ObjectCopyContext(void* root): TreeStructureContext<void*>(root)
        {}
    };

    struct ObjectValuePolicy
    {
        template<typename T, typename TContext>
        static void process(T&& value, TContext& context)
        {
            if(const auto current = context.parents.top())
            {
                value = *current;
            }
        }
    };

    struct ObjectEnumPolicy
    {
        template<typename T, typename TContext>
        static void process(T&& value, TContext& context)
        {
            if(const auto current = context.parents.top())
            {
                value = *current;
            }
        }
    };
}