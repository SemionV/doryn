#pragma once

#include <ryml.hpp>

namespace dory::serialization::yaml
{
    template<typename TContextPolicies>
    struct YamlContext: generic::serialization::TreeStructureContext<ryml::NodeRef, TContextPolicies>
    {
        explicit YamlContext(const ryml::NodeRef& root, generic::serialization::Context<TContextPolicies>&& otherContext):
            generic::serialization::TreeStructureContext<ryml::NodeRef, TContextPolicies>(root, otherContext)
        {}

        explicit YamlContext(const ryml::NodeRef& root, const generic::serialization::Context<TContextPolicies>& otherContext):
            generic::serialization::TreeStructureContext<ryml::NodeRef, TContextPolicies>(root, otherContext)
        {}

        YamlContext(const YamlContext& other) = default;
    };

    template<typename T>
    c4::csubstr toRymlCStr(const T& source)
    {
        return {source.data(), source.size()};
    }

    template<typename T>
    c4::substr toRymlStr(T& source)
    {
        return {source.data(), source.size()};
    }
}
