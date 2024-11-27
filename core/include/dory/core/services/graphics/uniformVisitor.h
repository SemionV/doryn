#pragma once

#include "dory/serialization/reflection.h"
#include "dory/math/linearAlgebra.h"

namespace dory::core::services::graphics
{
    template<typename TPolicies>
    class UniformVisitor
    {
        template<typename TContext>
        static void visit(const std::string_view& uniformName, const std::size_t uniformId, const math::Vector4f& uniformValue, TContext& context)
        {
            TPolicies::Vector4f::process(uniformName, uniformId, uniformValue, context);
        }

        template<typename TContext>
        static void visit(const std::string_view& uniformName, const std::size_t uniformId, const math::Matrix4x4f& uniformValue, TContext& context)
        {
            TPolicies::Matrix4x4f::process(uniformName, uniformId, uniformValue, context);
        }

        template<typename TContext>
        static void visit(const std::string_view& uniformName, const std::size_t uniformId, const resources::bindings::uniforms::Material& uniformValue, TContext& context)
        {
            TPolicies::MaterialBlock::process(uniformName, uniformId, uniformValue, context);
        }

        template<typename T, typename TContext>
        requires(std::is_fundamental_v<std::remove_reference_t<T>>)
        static void visit(const std::string_view& uniformName, const std::size_t uniformId, T&& uniformValue, TContext& context)
        {
            TPolicies::Value::process(uniformName, uniformId, std::forward<T>(uniformValue), context);
        }

        template<typename T, typename TContext>
        requires(std::is_class_v<std::decay_t<T>>)
        static void visit(const resources::bindings::uniforms::Uniforms& uniforms, TContext& context)
        {
            reflection::visitClassFields(uniforms, [](auto& memberValue, const std::string_view& memberName,
                                                    const std::size_t i, const std::size_t memberCount, TContext& context)
            {
                visit(memberName, i, memberValue, context);
            }, context);
        }
    };
}
