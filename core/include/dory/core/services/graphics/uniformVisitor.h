#pragma once

#include "dory/serialization/reflection.h"
#include "dory/math/linearAlgebra.h"

namespace dory::core::services::graphics
{
    template<typename TPolicies>
    class UniformVisitor
    {
    public:
        template<typename T,typename TContext>
        static void visit(const T& uniforms, TContext& context)
        {
            reflection::visitClassFields(uniforms, [](auto&& member, const std::size_t i, const std::size_t memberCount, TContext& context)
            {
                TPolicies::process(member.name, i, member.value, context);
            }, context);
        }
    };
}
