#pragma once

#include "dory/serialization/reflection.h"
#include "dory/math/linearAlgebra.h"

namespace dory::core::services::graphics
{
    template<typename TPolicies>
    class UniformVisitor
    {
    public:
        template<typename TContext>
        static void visit(const resources::bindings::uniforms::Uniforms& uniforms, TContext& context)
        {
            reflection::visitClassFields(uniforms, [](auto& memberValue, const std::string_view& memberName,
                                                    const std::size_t i, const std::size_t memberCount, TContext& context)
            {

                TPolicies::process(memberName, i, memberValue, context);
            }, context);
        }
    };
}
